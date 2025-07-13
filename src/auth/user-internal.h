#ifndef USER_INTERNAL_H
#define USER_INTERNAL_H

#include <stdint.h>
#include <stdio.h>
#include "../../include/privileges.h"

typedef struct user {
    uint32_t uid;
    char *username;
    char *password;
    Privileges privileges;
    struct user *next;
} user_t;

user_t *_create_user(char *username, char *passw, Privileges privileges);
void _free_user(user_t *user);

int _add_user(user_t *user);
int _remove_user(uint32_t uid);
int _find_users(const char *username, int limit, user_t **found);
user_t *_authenticate(const char *username, const char *passw);
int _count_users();
const char *_privilege_to_string(Privileges p);
char *_print_user(const user_t *user);

int _serialize_user(int fd, const user_t *u);
user_t *_deserialize_user(int fd);
int _save_users(const char *filename);
int _load_users(const char *filename);

#endif /* USER_INTERNAL_H */