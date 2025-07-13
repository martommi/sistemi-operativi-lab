#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

typedef int (*validator_fn)(const char *);

void serialize_string(FILE *fp, const char *str);
char *deserialize_string(FILE *fp);
const char *get_today();
int is_leap_year(int year);
int is_valid_date_format(const char *date);
int is_number(const char *str);
int is_valid_path(const char *path, int flags);
int can_read_file(const char *path);
int can_write_file(const char *path);
int read_line(FILE *fp, char *buffer, size_t size);
char *prompt_string(FILE *fp, const char *prompt);
char *prompt_validated_input(FILE *fp, const char *prompt, int (*validator)(const char *), const char *error_msg);
ssize_t read_all(int fd, void *buf, size_t count);
ssize_t write_all(int fd, const void *buf, size_t count);

#endif