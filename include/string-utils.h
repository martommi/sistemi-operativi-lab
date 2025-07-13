#ifndef STRING_UTILS_H
#define STRING_UTILS_H

int serialize_string(int fd, const char *str);
char *deserialize_string(int fd);

#endif