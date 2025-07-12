#include <stdlib.h>
#include <string.h>

#include "../../include/server-dispatcher.h"
#include "../../include/privileges.h"
#include "../../include/session.h"
#include "../auth/user-internal.h"
#include "../../include/user-lib.h"
#include "../../include/message.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/ticket.h"

#define DEFAULT_USERS_PATH "../../data/users/"
#define DEFAULT_TICKETS_PATH "../../data/tickets/"

typedef response_t *(*handler)(session_t *, message_t *);

static handler handlers[] = {
    [REQ_LOGIN]                      = handle_login,
    [REQ_LOGOUT]                     = handle_logout,
    [REQ_REGISTER_USER]             = handle_register_user,
    [REQ_REMOVE_USER]               = handle_remove_user,
    [REQ_FIND_USER]                 = handle_find_users,
    [REQ_SAVE_USERS]                = handle_save_users,
    [REQ_LOAD_USERS]                = handle_load_users,

    [REQ_OPEN_TICKET]               = handle_open_ticket,
    [REQ_DELETE_TICKET]             = handle_delete_ticket,
    [REQ_ASSIGN_TICKET]             = handle_assign_ticket,
    [REQ_UPDATE_TICKET_STATUS]      = handle_update_ticket_status,
    [REQ_GET_ALL_TICKETS]           = handle_get_all_tickets,
    [REQ_TICKET_FILTER_PRIORITY]    = handle_filter_by_priority, 
    [REQ_TICKET_FILTER_STATUS]      = handle_filter_by_status,
    [REQ_TICKET_FILTER_SUPPORT_AGENT] = handle_filter_by_support_agent,
    [REQ_TICKET_FILTER_TITLE]       = handle_filter_by_title,
    [REQ_TICKET_FILTER_DATE]        = handle_filter_by_date,
    [REQ_SAVE_TICKETS]              = handle_save_tickets,
    [REQ_LOAD_TICKETS]              = handle_load_tickets,

    [REQ_QUIT]                      = handle_quit
};

static size_t map_size = sizeof(handlers) / sizeof(handlers[0]);

int handle_request(session_t *session, request_t *req) {
    response_t *resp = NULL;

    if ((size_t)req->code >= map_size || handlers[req->code] == NULL) {
        resp = handle_invalid_request();
    } else {
        resp = handlers[req->code](session, req->payload);
    }

    send_response(session->fd, resp);

    int keep_session_alive = (resp->code == RESP_EXIT);
    free_response(resp);
    return keep_session_alive;
}

response_t *handle_invalid_request() {
    return create_response(RESP_INVALID_REQUEST, create_message_from_str("[SERVER] Invalid code."));
}

response_t *handle_login(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return login(session, msg->content[0], msg->content[1])
        ? create_response(RESP_OK, create_message_from_str("[LOGIN] Login successful."))
        : create_response(RESP_ERROR, create_message_from_str("[LOGIN] Invalid credentials."));
}

response_t *handle_logout(session_t *session, message_t *msg) {
    if (!is_logged_in(session)) return create_response(RESP_ERROR, create_message_from_str("[LOGOUT] No user is currently logged in."));
    logout(session);
    return create_response(RESP_OK, create_message_from_str("[LOGOUT] Logged out."));
}

response_t *handle_register_user(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return register_user(msg->content[0], msg->content[1], session->privileges)
        ? create_response(RESP_OK, create_message_from_str("[REGISTRATION] User registered successfully."))
        : create_response(RESP_ERROR, create_message_from_str("[REGISTRATION] User registration failed (already present?)."));
}

response_t *handle_remove_user(session_t *session, message_t *msg) {
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
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = atoi(msg->content[1]);
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

response_t *handle_open_ticket(session_t *session, message_t *msg) {
    if (msg->size < 3) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));    
    }

    return open_ticket(msg->content[0], msg->content[1], msg->content[2], 
        TICKET_PRIORITY_STANDARD, TICKET_STATUS_OPEN, NULL)

        ? create_response(RESP_OK, create_message_from_str("[TICKET] Ticket opened."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Ticket creation failed."));
}

response_t *handle_delete_ticket(session_t *session, message_t *msg) { 
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return delete_ticket(atoi(msg->content[0])) 
        ? create_response(RESP_OK, create_message_from_str("[TICKET] Ticket removed."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error removing ticket (non-existing?)."));
}

response_t *handle_assign_ticket(session_t *session, message_t *msg) { 
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT"));
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    ticket_t *ticket[1];
    get_ticket_by_tid(ticket, atoi(msg->content[0]));
    user_t *support_agent[1];
    find_users(msg->content[1], 1, support_agent);

    if (!ticket[0] || !support_agent[0]) {
        return create_response(RESP_ERROR, create_message_from_str("[TICKET] Ticket or support agent not found."));
    }

    return assign_ticket(ticket[0], support_agent[0])
        ? create_response(RESP_OK, create_message_from_str("[TICKET] ticket assigned."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error assigning ticket."));
}

response_t *handle_update_ticket_status(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    ticket_t *ticket[1];
    get_ticket_by_tid(ticket, atoi(msg->content[0]));

    return update_status(ticket[0], atoi(msg->content[1]))
        ? create_response(RESP_OK, create_message_from_str("[TICKET] Update succesful."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error updating."));
}

response_t *handle_get_all_tickets(session_t *session, message_t *msg) {
    int total = count_tickets();

    if (total <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    ticket_t **tickets = malloc(sizeof(ticket_t *) * total);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }

    get_all_tickets(tickets);

    char **content = malloc(sizeof(char *) * total);
    for (int i = 0; i < total; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }

    free(tickets);
    message_t *out = create_message(total, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_priority(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = atoi(msg->content[1]);
    int count;
    ticket_t **tickets = malloc(sizeof(ticket_t *) * limit);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }
    
    if ((count = get_tickets_by_priority(tickets, atoi(msg->content[0]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    count = count < limit ? count : limit;
    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_status(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = atoi(msg->content[1]);
    int count;
    ticket_t **tickets = malloc(sizeof(ticket_t *) * limit);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }
    
    if ((count = get_tickets_by_status(tickets, atoi(msg->content[0]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    count = count < limit ? count : limit;
    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_support_agent(session_t *session, message_t *msg) {
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    user_t *user[1];
    find_users(msg->content[0], 1, user);

    int limit = atoi(msg->content[1]);
    int count;
    ticket_t **tickets = malloc(sizeof(ticket_t *) * limit);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }
    
    if ((count = get_tickets_by_support_agent(tickets, user[0])) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    count = count < limit ? count : limit;
    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }

    free(tickets);
    message_t *out = create_message(count, content);
    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_title(session_t *session, message_t *msg) {
    if (msg->size < 3) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = atoi(msg->content[2]);
    int count;
    ticket_t **tickets = malloc(sizeof(ticket_t *) * limit);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }
    
    if ((count = get_tickets_by_title(tickets, msg->content[0], atoi(msg->content[1]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    count = count < limit ? count : limit;
    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_date(session_t *session, message_t *msg) {
    if (msg->size < 4) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = atoi(msg->content[3]);
    int count;
    ticket_t **tickets = malloc(sizeof(ticket_t *) * limit);
    if (!tickets) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Memory allocation failed."));
    }
    
    if ((count = get_tickets_by_date(tickets, atoi(msg->content[0]), msg->content[1], msg->content[2])) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found."));
    }

    count = count < limit ? count : limit;
    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_save_tickets(session_t *session, message_t *msg) {
    if (!is_logged_in(session)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] You must be logged in to perform this operation."));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return save_tickets(msg->content[0]) 
        ? create_response(RESP_OK, create_message_from_str("[TICKET] tickets saved on file."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] failed to save tickets."));
}

response_t *handle_load_tickets(session_t *session, message_t *msg) {
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    return load_tickets(msg->content[0]) 
        ? create_response(RESP_OK, create_message_from_str("[TICKET] tickets saved on file."))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] failed to save tickets."));
}

response_t *handle_quit(session_t *session, message_t *msg) {
    //generate day code
    char *name = "";
    save_users(strcat(DEFAULT_USERS_PATH, name));
    save_tickets(strcat(DEFAULT_TICKETS_PATH, name));

    free_message(msg);

    return create_response(RESP_EXIT, create_message_from_str("[SERVER] Users and tickets saved. You will now be disconnected."));
}