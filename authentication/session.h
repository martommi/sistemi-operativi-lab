#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#define DEFAULT_PORT 12345
#define LOCALHOST "127.0.0.1"

typedef struct user user_t;

typedef enum Priviliges { // qualche dubbio se rivedere i gruppi utente o questa TODO
    PRIVILEGES_ADMIN = (1 << 0),
    PRIVILEGES_GUEST = (1 << 1),
    PRIVILEGES_SUPPORT_AGENT = (1 << 2)
} Privileges;

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