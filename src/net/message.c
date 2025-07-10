#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

message_t *create_message_from_str(const char *str) {
    return create_message(1, (char *[]){ (char *)str });
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
    write(fd, &message->size, sizeof(size_t));

    for (size_t i = 0; i < message->size; i++) {
        size_t len = strlen(message->content[i]) + 1;
        write(fd, &len, sizeof(size_t));
        write(fd, message->content[i], len);
    }
}

void read_message(int fd, message_t **msg_out) { //TODO memory leak
    message_t *msg;
    if ((msg = (message_t *)malloc(sizeof(message_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    read(fd, &msg->size, sizeof(size_t));

    msg->content = malloc(sizeof(char*) * msg->size);
    for (size_t i = 0; i < msg->size; i++) {
        size_t len;
        read(fd, &len, sizeof(size_t));

        msg->content[i] = malloc(len);
        read(fd, msg->content[i], len);
    }

    *msg_out = msg;
}