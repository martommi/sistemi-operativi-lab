#include "../../include/file-utils.h"
#include <unistd.h>

ssize_t write_all(int fd, const void *buf, size_t count) {
    size_t written = 0;
    while (written < count) {
        ssize_t res = write(fd, (const char *)buf + written, count - written);
        if (res <= 0) return -1;
        written += res;
    }
    return written;
}

ssize_t read_all(int fd, void *buf, size_t count) {
    size_t read_bytes = 0;
    while (read_bytes < count) {
        ssize_t res = read(fd, (char *)buf + read_bytes, count - read_bytes);
        if (res <= 0) return -1;
        read_bytes += res;
    }
    return read_bytes;
}