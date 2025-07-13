#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user-internal.h"
#include "../../include/privileges.h"
#include "../../include/utils.h"

static user_t *head = NULL;
static uint32_t uid = 0;

user_t *_create_user(char *username, char *passw, Privileges privileges) {
    user_t *user = (user_t *)malloc(sizeof(user_t));
    if (!user) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    user->uid = uid++;

    user->username = strdup(username);
    if (!user->username) {
        perror("strdup username");
        free(user);
        exit(EXIT_FAILURE);
    }

    user->password = strdup(passw);
    if (!user->password) {
        perror("strdup password");
        free(user->username);
        free(user);
        exit(EXIT_FAILURE);
    }

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

    fprintf(stderr, "%s(): trying to remove non-existing user.\n", __func__);
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
    int count = 0;
    for (user_t *cur = head; cur; cur = cur->next)
        count++;
    return count;
}

const char *_privilege_to_string(Privileges p) {
    switch (1 << (31 - __builtin_clz(p))) { // isola il bit (non zero) più significativo a sinistra, cioè il privilegio più alto
        case PRIVILEGES_ADMIN: return "Admin";
        case PRIVILEGES_SUPPORT_AGENT: return "Support Agent";
        case PRIVILEGES_GUEST: return "Guest";
        default: return "N/A";
    }
}

char *_print_user(const user_t *u) {
    size_t len = snprintf(NULL, 0, 
        "ID: %d\n Username: %s\n Privileges: %s\n",
        u->uid,
        u->username,
        _privilege_to_string(u->privileges)
    );

    char *str = calloc(1, len + 1);
    if (str == NULL) {
        perror("calloc()");
        return NULL;
    }

    if (snprintf(str, len + 1, 
        "ID: %d\n Username: %s\n Privileges: %s\n",
        u->uid,
        u->username,
        _privilege_to_string(u->privileges)
    ) > len + 1) {

        fprintf(stderr, "%s() error: snprintf returned truncated result.", __func__);
        free(str);
        return NULL;
    }

    return str;
}

void _serialize_user(FILE *fp, const user_t *u) {
    fwrite(&u->uid, sizeof(uint32_t), 1, fp);
    serialize_string(fp, u->username);
    serialize_string(fp, u->password);
    fwrite(&u->privileges, sizeof(Privileges), 1, fp);
}

user_t *_deserialize_user(FILE *fp) {
    user_t *user = malloc(sizeof(user_t));
    if (!user) {
        perror("malloc()");
        return NULL;  // meglio non exit qui per maggiore flessibilità
    }

    if (fread(&user->uid, sizeof(uint32_t), 1, fp) != 1) {
        perror("fread uid");
        free(user);
        return NULL;
    }

    user->username = deserialize_string(fp);
    if (!user->username) {
        fprintf(stderr, "Failed to deserialize username\n");
        free(user);
        return NULL;
    }

    user->password = deserialize_string(fp);
    if (!user->password) {
        fprintf(stderr, "Failed to deserialize password\n");
        free(user->username);
        free(user);
        return NULL;
    }

    if (fread(&user->privileges, sizeof(Privileges), 1, fp) != 1) {
        perror("fread privileges");
        free(user->username);
        free(user->password);
        free(user);
        return NULL;
    }

    user->next = NULL;
    return user;
}

int _save_users(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen()");
        return -1;
    }

    int count = _count_users();
    fwrite(&count, sizeof(int), 1, fp);

    user_t *current = head;
    while (current) {
        _serialize_user(fp, current);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

int _load_users(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen()");
        return -1;
    }

    int count;
    fread(&count, sizeof(int), 1, fp);

    while (count > 0) {
        user_t *user = _deserialize_user(fp);
        _add_user(user);
        count--;
    }

    fclose(fp);
    return 1;
}