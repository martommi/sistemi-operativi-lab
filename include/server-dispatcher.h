#ifndef SERVER_DISPATCHER_H
#define SERVER_DISPATCHER_H

#include "session.h"
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
response_t *handle_save_users(session_t *session, message_t *msg);
response_t *handle_load_users(session_t *session, message_t *msg);

response_t *handle_open_ticket(session_t *session, message_t *msg);
response_t *handle_delete_ticket(session_t *session, message_t *msg);
response_t *handle_assign_ticket(session_t *session, message_t *msg);
response_t *handle_update_ticket_status(session_t *session, message_t *msg);
response_t *handle_get_all_tickets(session_t *session, message_t *msg);
response_t *handle_filter_by_priority(session_t *session, message_t *msg);
response_t *handle_filter_by_status(session_t *session, message_t *msg);
response_t *handle_filter_by_support_agent(session_t *session, message_t *msg);
response_t *handle_filter_by_title(session_t *session, message_t *msg);
response_t *handle_filter_by_date(session_t *session, message_t *msg);
response_t *handle_save_tickets(session_t *session, message_t *msg);
response_t *handle_load_tickets(session_t *session, message_t *msg);

#endif /* SERVER_DISPATCHER_H */