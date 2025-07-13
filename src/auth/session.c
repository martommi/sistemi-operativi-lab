#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/session.h"
#include "../../include/user-lib.h"
#include "user-internal.h"

session_t *create_session(int fd, user_t *user, int logged_in, Privileges privileges) {
    if (fd < 0) {
        fprintf(stderr, "%s(): invalid file descriptor.\n", __func__);
        return NULL;
    }

    session_t *session = malloc(sizeof(session_t));
    if (!session) {
        perror("malloc");
        return NULL;
    }

    memset(session, 0, sizeof(session_t));

    session->fd = fd;
    session->user = user;
    session->logged_in = logged_in;
    session->privileges = privileges;

    return session;
}

void end_session(session_t **session_ptr) {
    if (session_ptr == NULL || *session_ptr == NULL)
        return;

    session_t *session = *session_ptr;

    if (session->fd >= 0) {
        close(session->fd);
        session->fd = -1;
    }

    free(session);
    *session_ptr = NULL;
}

user_t *login(session_t *session, char *username, char *passw) {
    user_t *authenticated;
    if (!(authenticated = authenticate(username, passw))) {
        fprintf(stderr, "%s(): wrong credentials.", __func__);
        return NULL;
    }

    session->logged_in = 1;
    session->user = authenticated;
    session->privileges = authenticated->privileges;

    return authenticated;
}

void logout(session_t *session) {
    if (!session) return;

    session->logged_in = 0;
    session->privileges = PRIVILEGES_GUEST;
    session->user = NULL;
}

int is_logged_in(session_t *session) {
    return session->logged_in;
}

int session_has_privileges(session_t *session, Privileges privileges) {
    return (session->privileges & privileges) == privileges;
}