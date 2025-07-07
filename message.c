#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "message.h"

message_t *create_message(size_t size, char **content) {
    message_t *message;
    if (!(message = (message_t *)malloc(sizeof(message_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    message->size = size;
    message->content = content;

    return message;
}

void free_message(message_t *message) {
    for (int i = 0; i < message->size; i++) {
        if (message->content[i] == NULL) break;

        free(message->content[i]);
        message->content[i] = NULL;
    }

    free(message->content);
    message->content = NULL;
    free(message);
    message = NULL;
}

void write_message(int fd, message_t *message) {
    send(fd, &(message->size), sizeof(message->size), 0);    // Let the receiver know how much to read BEFORE sending any actual data

    for (int i = 0; i < message->size; i++) {
        send(fd, message->content[i], strlen(message->content[i]), 0); //TODO
        send(fd, "\n", 1, 0);
    }
}

void read_message(int fd, message_t **message) {
    //TODO recv
}