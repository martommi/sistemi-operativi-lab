#include <stdint.h>
#include <stdio.h>

#include "user-lib.h"
#include "user-internal.h"

int register_user(char *username, char *passw, Privileges privileges) {
    user_t *user = _create_user(username, passw, privileges);

    if (_add_user(user)) return 1;

    fprintf(stderr, "%s(): failed to register user %u (already exists?)\n", __func__, user->uid);
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

int user_in_group(const user_t *user, Privileges privileges) {
    return (user->privileges & privileges) != 0;
}

void user_add_group(user_t *user, Privileges privileges) {
    user->privileges |= privileges;
}

void user_remove_group(user_t *user, Privileges privileges) {
    user->privileges &= ~privileges;
}

void save_users(const char *filename) {
    _save_users(filename);
}

void load_users(const char *filename) {
    _load_users(filename);
}