#include <netinet/in.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/message.h"

message_t *create_message(size_t size, char **content) {
    message_t *message = calloc(1, sizeof(message_t));
    if (!message) {
        perror("calloc()");
        exit(EXIT_FAILURE);
    }

    message->size = size;
    message->content = content;

    return message;
}

message_t *create_message_from_str(const char *str) {
    char *copy = strdup(str);
    if (!copy) {
        perror("strdup()");
        exit(EXIT_FAILURE);
    }
    char **arr = malloc(sizeof(char *));
    if (!arr) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    arr[0] = copy;
    return create_message(1, arr);
}

void free_message(message_t *message) {
    if (!message) return;

    for (size_t i = 0; i < message->size; i++) {
        free(message->content[i]);  // Safe to call even if NULL
    }

    free(message->content);
    free(message);
}

void write_message(int fd, message_t *message) {
    uint32_t net_size = htonl(message->size);
    if (write(fd, &net_size, sizeof(uint32_t)) != sizeof(uint32_t)) {
        perror("write()");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < message->size; i++) {
        uint32_t len = strlen(message->content[i]) + 1;
        uint32_t net_len = htonl(len);

        if (write(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t)) {
            perror("write()");
            exit(EXIT_FAILURE);
        }

        if (write(fd, message->content[i], len) != len) {
            perror("write()");
            exit(EXIT_FAILURE);
        }
    }
}

void read_message(int fd, message_t **msg_out) {
    uint32_t net_size;
    if (read(fd, &net_size, sizeof(uint32_t)) != sizeof(uint32_t)) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    message_t *msg = calloc(1, sizeof(message_t));
    if (!msg) {
        perror("calloc()");
        exit(EXIT_FAILURE);
    }

    msg->size = ntohl(net_size);

    msg->content = calloc(msg->size, sizeof(char *));
    if (!msg->content) {
        perror("calloc()");
        free(msg);
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < msg->size; i++) {
        uint32_t net_len;
        if (read(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t)) {
            perror("read()");
            free_message(msg);
            exit(EXIT_FAILURE);
        }

        uint32_t len = ntohl(net_len);

        msg->content[i] = malloc(len);
        if (!msg->content[i]) {
            perror("malloc()");
            free_message(msg);
            exit(EXIT_FAILURE);
        }

        if (read(fd, msg->content[i], len) != len) {
            perror("read()");
            free_message(msg);
            exit(EXIT_FAILURE);
        }
    }

    *msg_out = msg;
}