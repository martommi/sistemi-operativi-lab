#include "./user.h"
#include "./user-lib.h"

int user_in_group(const user_t *user, uint8_t group) {
    return (user->groups & group) != 0;
}

void user_add_group(user_t *user, uint8_t group) {
    user->groups |= group;
}

void user_remove_group(user_t *user, uint8_t group) {
    user->groups &= ~group;
}