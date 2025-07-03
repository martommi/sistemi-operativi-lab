#ifndef USER_H
#define USER_H

#include <stdint.h>

#define GROUP_ADMIN            0x00
#define GROUP_CLIENT           0x01
#define GROUP_SUPPORT_AGENT    0x02

typedef struct user {
    uint32_t uid;
    char *username;
    char *password;
    uint8_t groups;
} user_t;

int user_in_group(const user_t *user, uint8_t group);
int user_add_group(user_t *user, uint8_t group);
int user_remove_group(user_t *user, uint8_t group);
char *user_show_groups(const user_t *user);

#endif /* USER_H */