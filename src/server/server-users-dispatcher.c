#include <stdio.h>
#include <stdlib.h>

#include "../../include/server-users-dispatcher.h"
#include "../../include/user-lib.h"
#include "../auth/user-internal.h"
#include "../../include/privileges.h"

response_t *handle_login(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return login(session, msg->content[0], msg->content[1]) != NULL
        ? create_response(RESP_OK, create_message_from_str("[LOGIN] Login successful."))
        : create_response(RESP_ERROR, create_message_from_str("[LOGIN] Invalid credentials."));
}

response_t *handle_logout(session_t *session, message_t *msg) {
    if (session == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (!is_logged_in(session)) return create_response(RESP_ERROR, create_message_from_str("[LOGOUT] No user is currently logged in."));
    logout(session);
    return create_response(RESP_OK, create_message_from_str("[LOGOUT] Logged out."));
}

response_t *handle_register_user(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return register_user(msg->content[0], msg->content[1], session->privileges)
        ? create_response(RESP_OK, create_message_from_str("[REGISTRATION] User registered successfully."))
        : create_response(RESP_ERROR, create_message_from_str("[REGISTRATION] User registration failed (already present?)."));
}

response_t *handle_remove_user(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (!session_has_privileges(session, PRIVILEGES_ADMIN)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing privileges: ADMIN."));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return remove_user(atoi(msg->content[0]))
        ? create_response(RESP_OK, create_message_from_str("[USER] User removed."))
        : create_response(RESP_ERROR, create_message_from_str("[USER] User removal failed (non-existing?)."));
}

response_t *handle_find_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = _count_users();
    user_t **users = malloc(sizeof(user_t *) * limit);
    int count = find_users(msg->content[0], limit, users);
    if (count <= 0) {
        free(users);
        return create_response(RESP_ERROR, create_message_from_str("[USER] Not found any users."));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = user_to_string(users[i]);
    }

    message_t *out = create_message(count, content);
    for (int i = 0; i < count; i++) free(content[i]);
    free(content);
    free(users);

    return create_response(RESP_OK, out);
}

response_t *handle_save_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }

    if (!is_logged_in(session)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] You must be logged in to perform this operation."));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return save_users(msg->content[0])
        ? create_response(RESP_OK, create_message_from_str("[SAVE] Users saved on file."))
        : create_response(RESP_ERROR, create_message_from_str("[SAVE] Failed to save."));
}

response_t *handle_load_users(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return NULL;
    }
    
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return load_users(msg->content[0])
        ? create_response(RESP_OK, create_message_from_str("[LOAD] Users loaded from file."))
        : create_response(RESP_ERROR, create_message_from_str("[LOAD] Couldn't load users."));
}
