#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/session.h"
#include "../../include/user-lib.h"
#include "../auth/user-internal.c"

session_t *create_session(int fd, user_t *user, int logged_in, Privileges privileges) {
    if (fd < 0) {
        fprintf(stderr,"%s(): file descriptor is a required field, please provide it.", __func__);
        exit(EXIT_FAILURE);
    }

    session_t *session;
    if (!(session = (session_t *)malloc(sizeof(session_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    memset(session, 0, sizeof(session_t));

    session->fd = fd;
    session->user = user;
    session->logged_in = logged_in;
    session->privileges = privileges;

    return session;
}

void end_session(session_t *session) {
    close(session->fd);
    free(session);
}

user_t *login(session_t *session, char *username, char *passw) {
    user_t *authenticated;
    if (!(authenticated = authenticate(username, passw))) {
        fprintf(stderr, "%s(): wrong credentials.", __func__);
        return 0;
    }

    session->logged_in = 1;
    session->user = authenticated;
    session->privileges = authenticated->privileges;

    return authenticated;
}

void logout(session_t *session) {
    session->logged_in = 0;
    session->privileges = PRIVILEGES_GUEST;
    session->user = NULL;
}

int is_logged_in(session_t *session) {
    return session->logged_in;
}

int session_has_privileges(session_t *session, Privileges privileges) {
    return session->privileges == privileges;
}