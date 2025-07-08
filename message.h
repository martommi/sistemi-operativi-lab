#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>

typedef struct message {
    size_t size;
    char **content;
} message_t;

message_t *create_message(size_t size, char **content);
message_t *create_message_from_str(const char *str);
void free_message(message_t *message);
void write_message(int fd, message_t *message);
void read_message(int fd, message_t **message);

#endif /* MESSAGE_H */