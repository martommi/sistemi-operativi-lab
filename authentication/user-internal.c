#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user-internal.h"

static user_t *head = NULL;
static uint32_t uid = 0;

user_t *_create_user(char *username, char *passw, Privileges privileges) {
    user_t *user;
    if (!(user = (user_t *)malloc(sizeof(user_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    user->uid = uid++;
    user->username = strdup(username);
    user->password = strdup(passw);
    user->privileges = privileges;
    user->next = NULL;

    return user;
}

void _free_user(user_t *user) {
    free(user->username);
    free(user->password);
    free(user);
}

int _add_user(user_t *user) {
    user_t *curr = head;

    while (curr) {
        if (curr->uid == user->uid) {
            // fprintf(stderr, "%s(): user with UID %u already registered.\n", __func__, user->uid);
            return 0;
        }
        
        curr = curr->next;
    }

    user->next = head;
    head = user;
    return 1;
}

int _remove_user(uint32_t uid) {
    user_t *curr = head;
    user_t *prev = NULL;

    while (curr) {
        if (curr->uid == uid) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }

            _free_user(curr);
            return 1;
        }

        prev = curr;
        curr = curr->next;
    }

    // fprintf(stderr, "%s(): trying to remove non-existing user.\n", __func__);
    return 0;
}

int _find_users(const char *username, int limit, user_t **found) { //TODO alloca
    user_t *curr = head;
    int count = 0;

    while (curr && count < limit) {
        if (strcmp(curr->username, username) == 0) {
            found[count++] = curr;
        }
        curr = curr->next;
    }

    return count;
}

user_t *_authenticate(const char *username, const char *passw) {
    user_t *curr = head;

    while (curr) {
        if (strcmp(curr->username, username) == 0 &&
            strcmp(curr->password, passw) == 0) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

int _count_users() {
    user_t *curr = head;
    int count = 0;

    while (curr) {
        count++;
        curr = curr->next;
    }

    return count;
}

int _save_users(const char *filename) {
    //todo
}

int _load_users(const char *filename) {
    //todo
}