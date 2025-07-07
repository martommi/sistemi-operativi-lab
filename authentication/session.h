#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>
#include "privileges.h"

#define DEFAULT_PORT 12345
#define LOCALHOST "127.0.0.1"

typedef struct user user_t;

typedef struct session {
    int fd;
    struct user *user;
    int logged_in;
    Privileges privileges;
} session_t;

session_t *start_session(int fd, user_t *user, int logged_in, Privileges privileges);
void end_session(session_t *session);

int login(session_t *session, char *username, char *passw);
void logout(session_t *session);
int is_logged_in(session_t *session);
int has_privileges(session_t *session, Privileges privileges);

#endif /* SESSION_H */