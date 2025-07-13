#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/message.h"
#include "../../include/file-utils.h"

message_t *create_message(uint32_t size, char **content) {
    if (content == NULL) {
        fprintf(stderr, "%s(): trying to create message with NULL content.", __func__);
        return NULL;
    }

    message_t *message = calloc(1, sizeof(message_t));
    if (!message) {
        perror("calloc()");
        return NULL;
    }

    message->size = size;
    message->content = content;

    return message;
}

message_t *create_message_from_str(const char *str) {
    if (str == NULL) {
        fprintf(stderr, "%s(): trying to create message with NULL content.", __func__);
        return NULL;
    }

    char *copy = strdup(str);
    if (!copy) {
        perror("strdup()");
        return NULL;
    }

    char **arr = malloc(sizeof(char *));
    if (!arr) {
        perror("malloc()");
        return NULL;
    }

    arr[0] = copy;
    return create_message(1, arr);
}

void free_message(message_t **message) {
    if (message == NULL || *message == NULL) return;

    for (uint32_t i = 0; i < (*message)->size; i++) {
        free((*message)->content[i]);
    }

    free((*message)->content);
    free(*message);
    *message = NULL;
}

int write_message(int fd, message_t *message) {
    if (message == NULL) {
        fprintf(stderr, "%s(): message is null.", __func__);
        return -1;
    }
    
    uint32_t net_size = htonl(message->size);
    if (write_all(fd, &net_size, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write failed.", __func__);
        return -1;
    }

    for (uint32_t i = 0; i < message->size; i++) {
        uint32_t len = strlen(message->content[i]) + 1;
        uint32_t net_len = htonl(len);

        if (write_all(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t)) {
            fprintf(stderr, "%s(): write failed.", __func__);
            return -1;
        }

        if (write_all(fd, message->content[i], len) != len) {
            fprintf(stderr, "%s(): write failed.", __func__);
            return -1;
        }
    }

    return 1;
}

int read_message(int fd, message_t **msg_out) {
    uint32_t net_size;
    if (read_all(fd, &net_size, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.", __func__);
        return -1;
    }

    message_t *msg = calloc(1, sizeof(message_t));
    if (!msg) {
        perror("calloc()");
        return -1;
    }

    msg->size = ntohl(net_size);

    msg->content = calloc(msg->size, sizeof(char *));
    if (!msg->content) {
        perror("calloc()");
        free(msg);
        return -1;
    }

    for (uint32_t i = 0; i < msg->size; i++) {
        uint32_t net_len;
        if (read_all(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t)) {
            perror("read()");
            free_message(msg);
            return -1;
        }

        uint32_t len = ntohl(net_len);

        msg->content[i] = malloc(len);
        if (!msg->content[i]) {
            perror("malloc()");
            free_message(msg);
            return -1;
        }

        if (read_all(fd, msg->content[i], len) != len) {
            perror("read()");
            free_message(msg);
            return -1;
        }
    }

    *msg_out = msg;
    return 1;
}