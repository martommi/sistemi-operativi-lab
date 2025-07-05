#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include "./user.h"

#define DEFAULT_PORT 12345
#define LOCALHOST "127.0.0.1"

typedef enum Priviliges {
    PRIVILEGES_ADMIN,
    PRIVILEGE_STANDARD,
    PRIVILEGES_SUPPORT_AGENT
} Privileges;

typedef struct session {
    int fd;
    struct user *user;
    int logged_in;
    Privileges privileges;
} session_t;

int login(char *usernmae, char *password);
int logout();

#endif /* SESSION_H */