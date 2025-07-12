#include <stdint.h>
#include <stdio.h>

#include "../../include/user-lib.h"
#include "user-internal.h"

int register_user(char *username, char *passw, Privileges privileges) {
    user_t *user = _create_user(username, passw, privileges);

    if (_add_user(user)) return 1;

    fprintf(stderr, "%s(): failed to register user %u (already exists?)\n", __func__, user->uid);
    _free_user(user);
    return 0;
}

int remove_user(uint32_t uid) {
    return _remove_user(uid);
}

int find_users(const char *username, int limit, user_t **found) {
    if (!username || limit <= 1 || !found) {
        fprintf(stderr, "%s(): invalid arguments\n", __func__);
        return -1;
    }

    return _find_users(username, limit, found);
}

user_t *authenticate(const char *username, const char *passw) {
    if (!username || !passw) {
        fprintf(stderr, "%s(): invalid arguments\n", __func__);
        return NULL;
    }
    
    return _authenticate(username, passw);
}

char *user_to_string(const user_t *user) {
    return _print_user(user);
}

int has_privileges(const user_t *user, Privileges privileges) {
    return (user->privileges & privileges) != 0;
}

void grant_privileges(user_t *user, Privileges privileges) {
    user->privileges |= privileges;
}

void revoke_privileges(user_t *user, Privileges privileges) {
    user->privileges &= ~privileges;
}

int save_users(const char *filename) {
   return _save_users(filename);
}

int load_users(const char *filename) {
   return _load_users(filename);
}