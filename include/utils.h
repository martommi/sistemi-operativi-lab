#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void serialize_string(FILE *fp, const char *str);
char *deserialize_string(FILE *fp);
const char *get_today();
int is_leap_year(int year);
int is_valid_date_format(const char *date);
int is_number(const char *str);
int is_valid_path(const char *path, int flags);

#endif