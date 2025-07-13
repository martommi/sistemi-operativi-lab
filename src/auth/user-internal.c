#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "user-internal.h"
#include "../../include/privileges.h"
#include "../../include/string-utils.h"
#include "../../include/file-utils.h"

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
        perror("strdup()");
        free(user);
        return NULL;
    }

    user->password = strdup(passw);
    if (!user->password) {
        perror("strdup()");
        free(user->username);
        free(user);
        return NULL;
    }

    user->privileges = privileges;
    user->next = NULL;

    return user;
}

void _free_user(user_t *user) {
    if (user == NULL) return;

    free(user->username);
    free(user->password);
    free(user);
}

int _add_user(user_t *user) {
    user_t *curr = head;

    while (curr) {
        if (curr->uid == user->uid) {
            fprintf(stderr, "%s(): user with UID %u already registered.\n", __func__, user->uid);
            return -1;
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
    return -1;
}

int _find_users(const char *username, int limit, user_t **found) { //TODO alloca dentro alla funzione
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
    switch (1 << (31 - __builtin_clz(p))) {    // isola il bit (non zero) più significativo a sinistra, cioè il privilegio più alto
        case PRIVILEGES_ADMIN: return "Admin";
        case PRIVILEGES_SUPPORT_AGENT: return "Support Agent";
        case PRIVILEGES_GUEST: return "Guest";
        default: return "N/A";
    }
}

char *_print_user(const user_t *u) {
    if (u == NULL) {
        fprintf(stderr, "%s(): trying to print null user.\n", __func__);
        return NULL;
    }

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
    ) > len) {

        fprintf(stderr, "%s() error: snprintf returned truncated result.\n", __func__);
        free(str);
        return NULL;
    }

    return str;
}

int _serialize_user(int fd, const user_t *u) {
    if (u == NULL) {
        fprintf(stderr, "%s(): null user.\n", __func__);
        return -1;
    }

    uint32_t uid_n = htonl(u->uid);
    if (write_all(fd, &uid_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write fail.\n", __func__);
        return -1;
    }

    if (!serialize_string(fd, u->username) || !serialize_string(fd, u->password)) {
        fprintf(stderr, "%s(): write failed.\n", __func__);
        return -1;
    }

    uint32_t priv_n = htonl(u->privileges);
    if (write_all(fd, &priv_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write failed.\n", __func__);
        return -1;
    }

    return 1;
}

user_t *_deserialize_user(int fd) {
    if (fd < 0) {
        fprintf(stderr, "%s(): invalid file descriptor.\n", __func__);
        return NULL;
    }

    user_t *user = malloc(sizeof(user_t));
    if (!user) {
        perror("malloc()");
        return NULL;
    }

    uint32_t uid_n;
    if (read_all(fd, &uid_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.", __func__);
        free(user);
        return NULL;
    }

    uint32_t uid = ntohl(uid_n);
    user->uid = uid;

    user->username = deserialize_string(fd);
    if (!user->username) {
        fprintf(stderr, "%s(): failed to deserialize username.\n", __func__);
        free(user);
        return NULL;
    }

    user->password = deserialize_string(fd);
    if (!user->password) {
        fprintf(stderr, "%s(): failed to deserialize password.\n", __func__);
        free(user->username);
        free(user);
        return NULL;
    }

    uint32_t priv_n;
    if (read_all(fd, &priv_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.\n", __func__);
        free(user->username);
        free(user->password);
        free(user);
        return NULL;
    }

    uint32_t priv = ntohl(priv_n);
    user->privileges = priv;

    user->next = NULL;
    return user;
}

int _save_users(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    uint32_t count_n = htonl(_count_users());
    if (write_all(fd, &count_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write failed.\n", __func__);
        close(fd);
        return -1;
    }

    user_t *current = head;
    while (current) {
        if (!_serialize_user(fd, current)) {
            close(fd);
            return -1;
        }

        current = current->next;
    }

    close(fd);
    return 1;
}

int _load_users(const char *filename) {
    int fd = open(filename, O_RDONLY, 0644);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    uint32_t count_n;
    if (read_all(fd, &count_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.\n", __func__);
        close(fd);
        return -1;
    }

    uint32_t count = ntohl(count_n);

    while (count > 0) {
        user_t *user = _deserialize_user(fd);
        if (!user) {
            close(fd);
            return -1;
        }

        _add_user(user);
        count--;
    }
    
    close(fd);
    return 1;
}