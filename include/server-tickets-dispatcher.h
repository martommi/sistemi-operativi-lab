#include "response.h"
#include "session.h"

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