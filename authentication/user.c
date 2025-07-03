#include "./user.h"
#include <string.h>

int user_login(char *username, char *password) {
    return 0;
}

int user_logout(const user_t *user) {
    return 0;
}

int user_in_group(const user_t *user, uint8_t group) {
    return (user->groups & group) != 0;
}

void user_add_group(user_t *user, uint8_t group) {
    user->groups |= group;
}

void user_remove_group(user_t *user, uint8_t group) {
    user->groups &= ~group;
}