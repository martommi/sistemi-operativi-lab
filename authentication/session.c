#include "./session.h"
#include "user-internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static user_t *head = NULL;

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
    logout(session);
    free(session);
}

int login(char *username, char *passw) { //TODO sposta in user
    session_t *session;
    user_t *current = head;

    while (current) {
        if (strcmp(current->username, username) == 0) {
            return session->logged_in = (strcmp(current->password, passw) == 0);
        }

        current = current->next;
    }

    return 0;
}

int logout(session_t *session) {
    return !(session->logged_in = 0);
}

int is_logged_in(session_t *session) {
    return session->logged_in;
}

int has_privileges(session_t *session, Privileges privileges) {
    return session->privileges == privileges;
}
