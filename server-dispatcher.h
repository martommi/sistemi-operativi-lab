#ifndef SERVER_DISPATCHER_H
#define SERVER_DISPATCHER_H

#include "authentication/session.h"
#include "message.h"
#include "request.h"
#include "response.h"

void handle_request(session_t *session, request_t *req);
response_t *handle_invalid_request();
response_t *handle_login(session_t *session, message_t *msg);
response_t *handle_logout(session_t *session, message_t *msg);

response_t *handle_register_user(session_t *session, message_t *msg);
response_t *handle_remove_user(session_t *session, message_t *msg);
response_t *handle_find_users(session_t *session, message_t *msg);
response_t *handle_user_in_group(session_t *session, message_t *msg);
response_t *handle_user_add_group(session_t *session, message_t *msg);
response_t *handle_user_remove_group(session_t *session, message_t *msg);
response_t *handle_save_users(session_t *session, message_t *msg);
response_t *handle_load_users(session_t *session, message_t *msg);

response_t *handle_get_all_tickets(session_t *session, message_t *msg);
response_t *handle_create_ticket(session_t *session, message_t *msg);
// aggiungi altri handler man mano

#endif /* SERVER_DISPATCHER_H */