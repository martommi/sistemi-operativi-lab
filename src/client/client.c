#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../../include/session.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/client-dispatcher.h"

static int sock = -1;

void handle_signal(int sig);

void handle_signal(int sig) {
    fprintf(stderr,"\n[CLIENT] Caught signal %d, closing connection...\n", sig);
    if (sock != -1) {
        close(sock);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    char target_ip[16];
    int port = DEFAULT_PORT;

    switch (argc) {
        case 3:
            port = atoi(argv[2]);
            // fallthrough

        case 2:
            strcpy(target_ip, argv[1]);
            break;

        case 1:
            strcpy(target_ip, LOCALHOST);
            break;

        default:
            printf("Usage: %s [IP ADDRESS] [PORT]\n", argv[0]);
            return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    printf("You are now connected with server.\n");

    print_menu();
    
    while (1) {
        request_t *req = NULL;
        response_t *resp = NULL;

        printf("> ");
        fflush(stdout);

        int retcode = compose_request(stdin, &req);

        if (retcode == 0) continue;   // Help menu was printed. No need to send a request.

        if (retcode < 0) {
            fprintf(stderr, "[CLIENT] failed to build request. Please try again or restart.\n");
            break;
        }

        if (send_request(sock, req) < 0) {
            fprintf(stderr, "[CLIENT] something went wrong communicating with server.\n");
            free_request(&req);
            continue;
        }

        free_request(&req);

        if (recv_response(sock, &resp) < 0) {
            fprintf(stderr, "[CLIENT] server didn't respond or sent invalid response.\n");
            continue;
        }
        
        print_response(stdout, resp);
        
        if (resp->code == RESP_EXIT) {
            free_response(&resp);
            break;
        }
        
        free_response(&resp);
    }
    
    close(sock);
    return 0;
}