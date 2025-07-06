#ifndef USER_INTERNAL_H
#define USER_INTERNAL_H

#include <stdint.h>

typedef struct user {
    uint32_t uid;
    char *username;
    char *password;
    uint8_t groups;
    struct user *next;
} user_t;

user_t *create_user(uint32_t uid, char *username, char *passw, uint8_t groups);
void free_user(user_t *user);

int add_user(user_t *user);
int _remove_user(user_t *user);
int _retrieve_users(const char *username, int limit, user_t **found);

#endif /* USER_INTERNAL_H */