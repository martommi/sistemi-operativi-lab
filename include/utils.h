#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void serialize_string(FILE *fp, const char *str);
char *deserialize_string(FILE *fp);

#endif