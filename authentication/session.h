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

session_t *start_session(int fd, user_t *user, int logged_in, Privileges privileges);
void end_session(session_t *session);

int login(char *usernmae, char *passw);
int logout(session_t *session);
int is_logged_in(session_t *session);
int has_privileges(session_t *session, Privileges privileges);

#endif /* SESSION_H */