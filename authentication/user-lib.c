#include <stdint.h>
#include <stdio.h>

#include "user-lib.h"
#include "user-internal.h"

int register_user(uint32_t uid, char *username, char *passw, uint8_t groups) {
    user_t *user = _create_user(uid, username, passw, groups);

    if (_add_user(user)) return 1;

    fprintf(stderr, "%s(): failed to register user %u (already exists?)\n", __func__, uid);
    _free_user(user);
    return 0;
}

int remove_user(user_t *user) {
    return _remove_user(user);
}

int find_users(const char *username, int limit, user_t **found) {
    if (!username || limit <= 1 || !found) {
        fprintf(stderr, "%s(): invalid arguments\n", __func__);
        return -1;
    }

    return _find_users(username, limit, found);
}

int authenticate(const char *username, const char *passw) {
    if (!username || !passw) {
        fprintf(stderr, "%s(): invalid arguments\n", __func__);
        return -1;
    }
    
    return _authenticate(username, passw);
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

void save_users(const char *filename) {
    _save_users(filename);
}

void load_users(const char *filename) {
    _load_users(filename);
}