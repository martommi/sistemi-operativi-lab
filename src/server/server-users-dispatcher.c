#include <stdio.h>
#include <stdlib.h>

#include "../../include/server-users-dispatcher.h"
#include "../../include/user-lib.h"
#include "../../include/privileges.h"

response_t *handle_login(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return login(session, msg->content[0], msg->content[1]) != NULL
        ? create_response(RESP_OK, create_message_from_str("[LOGIN] Login successful.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[LOGIN] Wrong credentials.\n"));
}

response_t *handle_logout(session_t *session, message_t *msg) {
    if (session == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }

    if (!is_logged_in(session)) return create_response(RESP_ERROR, create_message_from_str("[LOGOUT] No user is currently logged in.\n"));
    logout(session);
    return create_response(RESP_OK, create_message_from_str("[LOGOUT] Logged out.\n"));
}

response_t *handle_register_user(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return register_user(msg->content[0], msg->content[1], session->privileges) > 0
        ? create_response(RESP_OK, create_message_from_str("[REGISTRATION] User registered successfully.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[REGISTRATION] User registration failed (already present?).\n"));
}

response_t *handle_remove_user(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }

    if (!session_has_privileges(session, PRIVILEGES_ADMIN)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing privileges: ADMIN.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return remove_user(atoi(msg->content[0])) > 0
        ? create_response(RESP_OK, create_message_from_str("[USER] User removed.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[USER] User removal failed (non-existing?).\n"));
}

response_t *handle_find_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    user_t **users;
    int count = find_users(msg->content[0], &users);
    if (count <= 0) {
        free(users);
        return create_response(RESP_ERROR, create_message_from_str("[USER] Not found any users.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = user_to_string(users[i]);
    }

    message_t *out = create_message(count, content);
    free(users);

    return create_response(RESP_OK, out);
}

response_t *handle_save_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }

    if (!is_logged_in(session)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] You must be logged in to perform this operation.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return save_users(msg->content[0]) > 0
        ? create_response(RESP_OK, create_message_from_str("[SAVE] Users saved on file.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[SAVE] Failed to save.\n"));
}

response_t *handle_load_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));;
    }
    
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return load_users(msg->content[0]) > 0
        ? create_response(RESP_OK, create_message_from_str("[LOAD] Users loaded from file.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[LOAD] Couldn't load users.\n"));
}
