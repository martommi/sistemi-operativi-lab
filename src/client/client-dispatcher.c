#include "../../include/client-dispatcher.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef message_t *(*input_handler)(FILE *f);

input_handler input_handlers[] = {
    /* Authentication and user management */
    [REQ_LOGIN]                      = input_login,
    [REQ_LOGOUT]                     = input_logout,
    [REQ_REGISTER_USER]             = input_register_user,
    [REQ_REMOVE_USER]               = input_remove_user,
    [REQ_FIND_USER]                 = input_find_user,
    [REQ_SAVE_USERS]                = input_save_users,
    [REQ_LOAD_USERS]                = input_load_users,

    /* Ticket management */
    [REQ_OPEN_TICKET]               = input_open_ticket,
    [REQ_DELETE_TICKET]            = input_delete_ticket,
    [REQ_ASSIGN_TICKET]            = input_assign_ticket,
    [REQ_UPDATE_TICKET_STATUS]     = input_update_ticket_status,
    [REQ_GET_ALL_TICKETS]          = input_get_all_tickets,
    [REQ_TICKET_FILTER_PRIORITY]   = input_ticket_filter_priority,
    [REQ_TICKET_FILTER_STATUS]     = input_ticket_filter_status,
    [REQ_TICKET_FILTER_SUPPORT_AGENT] = input_ticket_filter_support_agent,
    [REQ_TICKET_FILTER_TITLE]      = input_ticket_filter_title,
    [REQ_TICKET_FILTER_DATE]       = input_ticket_filter_date,
    [REQ_SAVE_TICKETS]             = input_save_tickets,
    [REQ_LOAD_TICKETS]             = input_load_tickets,

    /* Other */
    [REQ_QUIT]                      = input_quit
};

static size_t map_size = sizeof(input_handlers) / sizeof(input_handlers[0]);

void print_menu() {
    char *menu = "\
    \
    \
    \
    \
    \
    \
    \
    \
    ";

    printf("%s", menu);
}

int compose_request(FILE *fp, request_t **req_ptr) {
    int choice;
    char buffer[256];

    if (fscanf(fp, "%d", &choice) != 1) {
        fprintf(stderr, "[CLIENT] Invalid input.\n");
        *req_ptr = NULL;
        return -1;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);    /* flush newline from input buffer */

    if (choice < 0 || choice >= map_size || input_handlers[choice] == NULL) {
        fprintf(stderr, "[CLIENT] Invalid or unknown request code: %d\n", choice);
        *req_ptr = NULL;
        return -1;
    }

    message_t *msg = input_handlers[choice](fp);

    *req_ptr = create_request((RequestCode)choice, msg);
    return 1;
}

message_t *input_login(FILE *fp) {
    // TODO
}

message_t *input_logout(FILE *fp) {
    // TODO
}

message_t *input_register_user(FILE *fp) {
    // TODO
}

message_t *input_remove_user(FILE *fp) {
    // TODO
}

message_t *input_find_user(FILE *fp) {
    // TODO
}

message_t *input_save_users(FILE *fp) {
    // TODO
}

message_t *input_load_users(FILE *fp) {
    // TODO
}

message_t *input_open_ticket(FILE *fp) {
    // TODO
}

message_t *input_delete_ticket(FILE *fp) {
    // TODO
}

message_t *input_assign_ticket(FILE *fp) {
    // TODO
}

message_t *input_update_ticket_status(FILE *fp) {
    // TODO
}

message_t *input_get_all_tickets(FILE *fp) {
    // TODO
}

message_t *input_ticket_filter_priority(FILE *fp) {
    // TODO
}

message_t *input_ticket_filter_status(FILE *fp) {
    // TODO
}

message_t *input_ticket_filter_support_agent(FILE *fp) {
    // TODO
}

message_t *input_ticket_filter_title(FILE *fp) {
    // TODO
}

message_t *input_ticket_filter_date(FILE *fp) {
    // TODO
}

message_t *input_save_tickets(FILE *fp) {
    // TODO
}

message_t *input_load_tickets(FILE *fp) {
    // TODO
}

message_t *input_quit(FILE *fp) {
    // TODO
}