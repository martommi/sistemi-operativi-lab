#ifndef USER_LIB_H
#define USER_LIB_H

#include <stdint.h>
#include "./user.h"

#define GROUP_ADMIN            0x00
#define GROUP_CLIENT           0x01
#define GROUP_SUPPORT_AGENT    0x02

int user_in_group(const user_t *user, uint8_t group);
void user_add_group(user_t *user, uint8_t group);
void user_remove_group(user_t *user, uint8_t group);

#endif /* USER_LIB_H */