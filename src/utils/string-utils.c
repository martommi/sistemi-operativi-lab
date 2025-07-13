#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/string-utils.h"
#include "../../include/file-utils.h"

int serialize_string(int fd, const char *str) {
    if (!str) {
        fprintf(stderr, "serialize_string: null pointer\n");
        return -1;
    }

    uint32_t len = strlen(str);
    uint32_t net_len = htonl(len);

    if (write_all(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t) ||
        write_all(fd, str, len) != len) {
        // error
        return -1;
    }

    return 1;
}

char *deserialize_string(int fd) {
    uint32_t net_len;

    if (read_all(fd, &net_len, sizeof(uint32_t)) != sizeof(uint32_t)) {
        perror("read_all(len)");
        return NULL;
    }

    uint32_t len = ntohl(net_len);

    char *str = malloc(len + 1);
    if (!str) {
        perror("malloc()");
        return NULL;
    }

    if (read_all(fd, str, len) != (ssize_t)len) {
        perror("read_all(str)");
        free(str);
        return NULL;
    }

    str[len] = '\0';
    return str;
}