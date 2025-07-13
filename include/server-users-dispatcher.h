#include "response.h"
#include "session.h"

response_t *handle_login(session_t *session, message_t *msg);
response_t *handle_logout(session_t *session, message_t *msg);

response_t *handle_register_user(session_t *session, message_t *msg);
response_t *handle_remove_user(session_t *session, message_t *msg);
response_t *handle_find_users(session_t *session, message_t *msg);
response_t *handle_save_users(session_t *session, message_t *msg);
response_t *handle_load_users(session_t *session, message_t *msg);