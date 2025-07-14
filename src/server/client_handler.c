#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/session.h"
#include "../../include/privileges.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/server-dispatcher.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <socket_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Retrieve socket descriptor. */
    char *endptr;
    errno = 0;
    long result = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0' || result < 0) {
        fprintf(stderr, "Invalid value for <socket_fd>\n");
        exit(EXIT_FAILURE);
    }

    int client_sock = (int)result;
    if (client_sock < 0) {
        fprintf(stderr, "Invalid socket file descriptor.\n");
        exit(EXIT_FAILURE);
    }

    session_t *session = create_session(client_sock, NULL, 0, PRIVILEGES_GUEST);

    init_test_users();

    request_t *req;
    while (1) {
        req = NULL;
        if (recv_request(client_sock, &req) < 0) {
            fprintf(stderr, "%s(): failed to receive request.\n", __func__);

            if (fcntl(client_sock, F_GETFD) != -1 || errno != EBADF)
                continue;  // error but socket is still open -> keep reading
            break; // closed socket -> exit
        }

        if (handle_request(session, req) <= 0) {    /* if it receives an exit code, ends session */
            free_request(&req);
            break;
        }

        free_request(&req);
    }
    
    end_session(&session);
}