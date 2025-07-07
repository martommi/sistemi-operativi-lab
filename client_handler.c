#include <stdio.h>
#include <stdlib.h>

#include "authentication/session.h"
#include "authentication/privileges.h"

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
    start_session(session);
    end_session(session);
}