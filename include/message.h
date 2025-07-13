#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>
#include <stdint.h>

typedef struct message {
    uint32_t size;   /* uint32_t instead of size_t for better network byte order translation. */
    char **content;
} message_t;

/* message content string array allocation is caller responsibility. The free is library responsibility. */
message_t *create_message(uint32_t size, char **content);
message_t *create_message_from_str(const char *str);
/* Caller should allocate memory for message content but rely on lib function to free it. */
void free_message(message_t **message);
int write_message(int fd, message_t *message);
/* Memory allocation responsibility is library's */
int read_message(int fd, message_t **message);

#endif /* MESSAGE_H */