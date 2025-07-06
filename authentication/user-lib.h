#ifndef USER_LIB_H
#define USER_LIB_H

#include <stdint.h>

typedef struct user user_t;    // not referencing user.h to hide details of struct user

#define GROUP_ADMIN            (1 << 0)
#define GROUP_CLIENT           (1 << 1)
#define GROUP_SUPPORT_AGENT    (1 << 2)

int register_user(uint32_t uid, char *username, char* passw, uint8_t groups);    // wrapper for create + add
int remove_user(user_t *user);
int retrieve_users(const char *username, int limit, user_t **found);

int user_in_group(const user_t *user, uint8_t group);
void user_add_group(user_t *user, uint8_t group);
void user_remove_group(user_t *user, uint8_t group);

void save_users(char *filename);
void load_users(char *filename);

#endif /* USER_LIB_H */