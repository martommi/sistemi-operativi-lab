#ifndef USER_LIB_H
#define USER_LIB_H

#include <stdint.h>
#include "privileges.h"

typedef struct user user_t;    // not referencing user-internal.h to hide details of struct user

int register_user(char *username, char* passw, Privileges privileges);    // wrapper for create + add
int remove_user(uint32_t uid);
int find_users(const char *username, int limit, user_t **found);
user_t *authenticate(const char *username, const char* passw);

int user_in_group(const user_t *user, Privileges privileges);
void user_add_group(user_t *user, Privileges privileges);
void user_remove_group(user_t *user, Privileges privileges);

void save_users(const char *filename);
void load_users(const char *filename);

#endif /* USER_LIB_H */