#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

ssize_t read_all(int fd, void *buf, size_t count);
ssize_t write_all(int fd, const void *buf, size_t count);
int read_line(FILE *fp, char *buffer, size_t size);

#endif