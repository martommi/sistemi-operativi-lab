#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../include/session.h"

#define MAX_CLIENTS 5
#define CLIENT_HANDLER_NAME "client_handler"
#define CLIENT_HANDLER_PATH "./bin/internal/" CLIENT_HANDLER_NAME

static int server_sock;    /* Just for handling signals */

void handle_signal(int sig);

void handle_signal(int sig) {
    printf("\n[SERVER] SIGINT received. Shutting down server...\n");

    pid_t pgid = getpgrp();
    printf("[SERVER] Sending SIGTERM to process group %d\n", pgid);    /* If interrupted, kill all children (in the same group)*/
    killpg(pgid, SIGTERM);

    close(server_sock);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_sock;
    int port = DEFAULT_PORT;

    signal(SIGINT, handle_signal);

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {    /* Socket creation */
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    if (argc > 2) {
        printf("Usage: %s [PORT]\n", argv[0]);
        return -1;
    }

    if (argc == 2) {
        port = atoi(argv[1]);    /* Define custom port */
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {    /* Bind the socket */
        perror("bind()");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {    /* Listening for clients */
        perror("listen()");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server now listening on port: %d\n", port);

    signal(SIGCHLD, SIG_IGN);    /* Ignore children, automatic cleanup */
    setpgid(0, 0);

    while (1) {    /* Server loop */
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len)) == -1) {
            perror("accept()");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            setpgid(0, getppid());    /* Same group as server for controlled termination */

            char *client_ip = inet_ntoa(client_addr.sin_addr);
            printf("Client @ %s connected.\n", client_ip);

            close(server_sock);

            char sock_str[16];
            snprintf(sock_str, sizeof(sock_str), "%d", client_sock);    /* Print client socket descriptor into a string */

            execl(CLIENT_HANDLER_PATH, CLIENT_HANDLER_NAME, sock_str, (char *)NULL);

            /* execl failure */
            perror("execl()");
            close(client_sock);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            close(client_sock);
        } else {
            perror("fork()");
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}