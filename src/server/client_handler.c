#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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

    int client_sock = atoi(argv[1]); /* Retrieve socket descriptor */

    if (client_sock < 0) {
        fprintf(stderr, "Invalid socket file descriptor.\n");
        exit(EXIT_FAILURE);
    }

    session_t *session = create_session(client_sock, NULL, 0, PRIVILEGES_GUEST);

    init_test_users();

    while (1) {
        request_t *req = NULL;
        recv_request(client_sock, &req);

        if (!handle_request(session, req)) {    /* if it receives an exit code, ends session */
            free_request(req);
            break;
        }

        free_request(req);
    }
    
    end_session(&session);
}