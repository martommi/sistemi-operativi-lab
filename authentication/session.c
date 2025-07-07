#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./session.h"
#include "user-lib.h"

session_t *start_session(int fd, user_t *user, int logged_in, Privileges privileges) {
    if (fd < 0) {
        printf("start_session(): file descriptor is a required field, please provide it.");
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
    free(session);
}

int login(session_t *session, char *username, char *passw) {
    if (!authenticate(username, passw)) {
        fprintf(stderr, "%s(): wrong credentials.", __func__);
        return 0;
    }

    return session->logged_in = 1;
}

void logout(session_t *session) {
    session->logged_in = 0;
}

int is_logged_in(session_t *session) {
    return session->logged_in;
}

int has_privileges(session_t *session, Privileges privileges) {
    return session->privileges == privileges;
}