#include "../../include/server-tickets-dispatcher.h"
#include "../../include/ticket.h"
#include "../../include/user-lib.h"

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
    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = count_tickets();
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
    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = count_tickets();
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
    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    user_t *user[1];
    find_users(msg->content[0], 1, user); //TODO no limit

    int limit = count_tickets();
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
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = count_tickets();
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

response_t *handle_filter_by_date(session_t *session, message_t *msg) { // TODO modifica in base al numero di argomenti
    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args."));
    }

    int limit = count_tickets();
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