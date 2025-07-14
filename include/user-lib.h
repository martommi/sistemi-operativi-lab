#ifndef USER_LIB_H
#define USER_LIB_H

#include <stdint.h>
#include "privileges.h"

typedef struct user user_t;    // not referencing user-internal.h to hide details of struct user

int register_user(char *username, char* passw, Privileges privileges);    // wrapper for create + add
int remove_user(uint32_t uid);
int find_users(const char *username, user_t ***found);
user_t *authenticate(const char *username, const char* passw);
char *user_to_string(const user_t *user);

int has_privileges(const user_t *user, Privileges privileges);
void grant_privileges(user_t *user, Privileges privileges);
void revoke_privileges(user_t *user, Privileges privileges);

int save_users(const char *filename);
int load_users(const char *filename);

#endif /* USER_LIB_H */