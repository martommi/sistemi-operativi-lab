#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/session.h"
#include "../../include/privileges.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/server-dispatcher.h"

static session_t *session = NULL;

void handle_signal(int sig);

void handle_signal(int sig) {
    fprintf(stderr, "\nCaught signal %d, shutting down...\n", sig);

    handle_quit(session, NULL);

    if (session) {
        end_session(&session);
    }
    
    exit(EXIT_SUCCESS);
}

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

    /* Handling signals */
    signal(SIGINT, handle_signal);    // saves the state if interrupted.

    session = create_session(client_sock, NULL, 0, PRIVILEGES_GUEST);

    init_test_users();    // for testing

    request_t *req;
    while (1) {
        req = NULL;
        if (recv_request(client_sock, &req) < 0) {
            fprintf(stderr, "%s(): failed to receive request.\n", __func__);
            break;
        }

        if (handle_request(session, req) <= 0) {    /* if it receives an exit code, ends session */
            free_request(&req);
            break;
        }

        free_request(&req);
    }
    
    end_session(&session);   // closes the socket
    return EXIT_SUCCESS;
}