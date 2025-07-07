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

user_t *_create_user(char *username, char *passw, uint8_t groups);
void _free_user(user_t *user);

int _add_user(user_t *user);
int _remove_user(user_t *user);
int _find_users(const char *username, int limit, user_t **found);
int _authenticate(const char *username, const char *passw);
int _count_users();

void _save_users(const char *filename);
void _load_users(const char *filename);

#endif /* USER_INTERNAL_H */