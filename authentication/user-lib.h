#ifndef USER_LIB_H
#define USER_LIB_H

#include <stdint.h>
#include "./user.h"

#define GROUP_ADMIN            0x00
#define GROUP_CLIENT           0x01
#define GROUP_SUPPORT_AGENT    0x02

user_t *create_user(uint32_t uid, char *username, char* passw, uint8_t groups);
void free_user(user_t *user);
int count_users();
void init_users();

int add_user(user_t *user);
int remove_user(user_t *user);

int user_in_group(const user_t *user, uint8_t group);
void user_add_group(user_t *user, uint8_t group);
void user_remove_group(user_t *user, uint8_t group);

void save_users(char *filename);
void load_users(char *filename);

#endif /* USER_LIB_H */