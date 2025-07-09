#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./authentication/session.h"
#include "protocol.h"
#include "request.h"
#include "response.h"

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

    request_t *req; //TODO alloca dentro al ciclo
    response_t *resp;

    while (1) {
        //ciclo di richieste TODO
        send_request(sock, req);
        recv_response(sock, &resp);
        // handle_response
    }
    
    close(sock);
    return 0;
}