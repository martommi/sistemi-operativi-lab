#ifndef USER_H
#define USER_H

#include <stdint.h>

typedef struct user {
    uint32_t uid;
    char *username;
    char *password;
    uint8_t groups;
    struct user *next;
} user_t;

#endif /* USER_H */