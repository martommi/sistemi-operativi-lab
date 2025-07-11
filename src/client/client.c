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

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    char target_ip[16];
    int port = DEFAULT_PORT;
    int sock;

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

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    printf("You are connected with the server.");

    while (1) {
        request_t *req = NULL;
        response_t *resp = NULL;

        print_menu(stdout);
        printf("> ");
        fflush(stdout);

        compose_request(stdin, &req);
        send_request(sock, req);
        free_request(req);

        recv_response(sock, &resp);
        print_response(stdout, resp);
        
        if (resp->code == RESP_EXIT) {
            free_response(resp);
            break;
        }
        
        free_response(resp);
    }
    
    close(sock);
    return 0;
}