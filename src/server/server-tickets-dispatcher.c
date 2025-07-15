#include "../../include/server-tickets-dispatcher.h"
#include "../../include/ticket.h"
#include "../../include/user-lib.h"

response_t *handle_open_ticket(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 3) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return open_ticket(msg->content[0], msg->content[1], msg->content[2],
        TICKET_PRIORITY_STANDARD, TICKET_STATUS_OPEN, NULL) > 0

        ? create_response(RESP_OK, create_message_from_str("[TICKET] Ticket opened.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Ticket creation failed.\n"));
}

response_t *handle_delete_ticket(session_t *session, message_t *msg) { 
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return delete_ticket(atoi(msg->content[0])) > 0
        ? create_response(RESP_OK, create_message_from_str("[TICKET] Ticket removed.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error removing ticket (non-existing?).\n"));
}

response_t *handle_assign_ticket(session_t *session, message_t *msg) { 
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT\n"));
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    ticket_t **ticket;
    get_ticket_by_tid(&ticket, atoi(msg->content[0]));
    user_t **support_agent;
    find_users(msg->content[1], &support_agent);

    if (!ticket[0] || !support_agent[0]) {
        return create_response(RESP_ERROR, create_message_from_str("[TICKET] Ticket or support agent not found.\n"));
    }

    return assign_ticket(ticket[0], support_agent[0]) > 0
        ? create_response(RESP_OK, create_message_from_str("[TICKET] ticket assigned.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error assigning ticket.\n"));
}

response_t *handle_update_ticket_status(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }
    
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT\n"));
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    ticket_t **ticket;
    get_ticket_by_tid(&ticket, atoi(msg->content[0]));

    return update_status(ticket[0], atoi(msg->content[1])) > 0
        ? create_response(RESP_OK, create_message_from_str("[TICKET] Update succesful.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] Error updating.\n"));
}

response_t *handle_get_all_tickets(session_t *session, message_t *msg) {
    if (session == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    int total = count_tickets();

    if (total <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    ticket_t **tickets;
    get_all_tickets(&tickets);

    char **content = malloc(sizeof(char *) * total);
    for (int i = 0; i < total; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }

    free(tickets);
    message_t *out = create_message(total, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_priority(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    int count;
    ticket_t **tickets;
    
    if ((count = get_tickets_by_priority(&tickets, atoi(msg->content[0]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_status(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    int count;
    ticket_t **tickets;
    
    if ((count = get_tickets_by_status(&tickets, atoi(msg->content[0]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_support_agent(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    user_t **user;
    if (find_users(msg->content[0], &user) <= 0) {
        return create_response(RESP_ERROR, create_message_from_str("[TICKET] No support agent found.\n"));
    }

    int count;
    ticket_t **tickets;
    
    if ((count = get_tickets_by_support_agent(&tickets, user[0])) <= 0) {
        return create_response(RESP_ERROR, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }

    free(tickets);
    message_t *out = create_message(count, content);
    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_title(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    int count;
    ticket_t **tickets;
    
    if ((count = get_tickets_by_title(&tickets, msg->content[0], atoi(msg->content[1]))) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_filter_by_date(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (msg->size < 2) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    int count;
    ticket_t **tickets;
    int mode = atoi(msg->content[0]);
    const char *arg1 = msg->content[1];
    const char *arg2 = msg->content[2] ? msg->content[2] : NULL;
    
    if ((count = get_tickets_by_date(&tickets, mode, arg1, arg2)) <= 0) {
        return create_response(RESP_OK, create_message_from_str("[TICKET] no ticket found.\n"));
    }

    char **content = malloc(sizeof(char *) * count);
    for (int i = 0; i < count; i++) {
        content[i] = ticket_to_string(tickets[i]);
    }
    
    free(tickets);
    message_t *out = create_message(count, content);

    return create_response(RESP_OK, out);
}

response_t *handle_save_tickets(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }

    if (!is_logged_in(session)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] You must be logged in to perform this operation.\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return save_tickets(msg->content[0]) > 0
        ? create_response(RESP_OK, create_message_from_str("[TICKET] tickets saved on file.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] failed to save tickets.\n"));
}

response_t *handle_load_tickets(session_t *session, message_t *msg) {
    if (msg == NULL || session == NULL || msg->content == NULL) {
        fprintf(stderr, "%s(): null pointer.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Something went wrong.\n"));
    }
    
    if (!session_has_privileges(session, PRIVILEGES_ADMIN | PRIVILEGES_SUPPORT_AGENT)) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Missing required privileges: ADMIN or SUPPORT\n"));
    }

    if (msg->size < 1) {
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Invalid args.\n"));
    }

    return load_tickets(msg->content[0]) > 0
        ? create_response(RESP_OK, create_message_from_str("[TICKET] tickets saved on file.\n"))
        : create_response(RESP_ERROR, create_message_from_str("[TICKET] failed to save tickets.\n"));
}