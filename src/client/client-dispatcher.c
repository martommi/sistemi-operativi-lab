#include "../../include/client-dispatcher.h"
#include "../../include/utils.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

static size_t map_size = sizeof(input_handlers) / sizeof(input_handler);

void print_menu() {
    char *menu = "Choose an option:\
    1- Login \n\
    2- Logout \n\
    3- Register user \n\
    4- Remove user \n\
    5- Find user \n\
    6- Save users on file \n\
    7- Load users from file \n\
    8- Open ticket \n\
    9- Delete ticket \n\
    10- Assign ticket \n\
    11- Update ticket status \n\
    12- Print all tickets \n\
    13- Filter tickets by priority \n\
    14- Filter tickets by status \n\
    15- Filter tickets by support agent \n\
    16- Filter tickets by title \n\
    17- Filter tickets by date \n\
    18- Save tickets on file \n\
    19- Load tickets from file \n\
    20- Disconnect \n\
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
    char *username = prompt_string(fp, "> username: ");
    char *password = prompt_string(fp, "> password: ");
    if (!username || !password) return NULL;
    char *content[] = { username, password };
    return create_message(2, content);
}

message_t *input_logout(FILE *fp) {
    return NULL;    /* No input required */
}

message_t *input_register_user(FILE *fp) {
    return input_login(fp);   //TODO non mi piace che si chiamano
}

message_t *input_remove_user(FILE *fp) {
    char *uid = prompt_validated_input(fp, "> uid of user to delete: ", is_number, "[REMOVE] Invalid UID. Must be a number.");
    if (!uid) return NULL;
    char *content[] = { uid };
    return create_message(1, content);
}

message_t *input_find_user(FILE *fp) {
    char *username = prompt_string(fp, "> find username: ");
    if (!username) return NULL;
    char *content[] = { username };
    return create_message(1, content);
}

message_t *input_save_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_write_file, "[SAVE] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}

message_t *input_load_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_read_file, "[LOAD] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}

message_t *input_open_ticket(FILE *fp) {
    char *title = prompt_string(fp, "> ticket title: ");
    char *description = prompt_string(fp, "> ticket description: ");
    if (!title || !description) return NULL;
    char *content[] = { title, description };
    return create_message(2, content);
}

message_t *input_delete_ticket(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[DELETE] Invalid ticket ID.");
    if (!tid) return NULL;
    char *content[] = { tid };
    return create_message(1, content);
}

message_t *input_assign_ticket(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[ASSIGN] Invalid ticket ID.");
    char *agent = prompt_string(fp, "> assign to (agent username): ");
    if (!tid || !agent) return NULL;
    char *content[] = { tid, agent };
    return create_message(2, content);
}

message_t *input_update_ticket_status(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[UPDATE] Invalid ticket ID.");
    printf("Select new status: \n\
        1- Open\n\
        2- Ongoing\n\
        3- Closed\n\
        ");
    char *status = prompt_validated_input(fp, "> new status: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!tid || !status) return NULL;
    char *content[] = { tid, status };
    return create_message(2, content);
}

message_t *input_get_all_tickets(FILE *fp) {
    return NULL;
}

message_t *input_ticket_filter_priority(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[UPDATE] Invalid ticket ID.");
    printf("Filter by priority: \n\
        1- Low\n\
        2- Standard\n\
        3- High\n\
        ");
    char *status = prompt_validated_input(fp, "> new status: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!tid || !status) return NULL;
    char *content[] = { tid, status };
    return create_message(2, content);
}

message_t *input_ticket_filter_support_agent(FILE *fp) {
    char *agent = prompt_string(fp, "> support agent (username): ");
    if (!agent) return NULL;
    char *content[] = { agent };
    return create_message(1, content);
}

message_t *input_ticket_filter_title(FILE *fp) {
    char *title = prompt_string(fp, "> ticket title: ");
    printf("Tile matching mode: \n\
        1- Exact Match\n\
        2- Contains\n\
        3- Starts With\n\
        ");
    char *mode = prompt_validated_input(fp, "> mode: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!title || !mode) return NULL;
    char *content[] = { title, mode};
    return create_message(2, content);
}

message_t *input_ticket_filter_date(FILE *fp) {
    printf("Date matching mode:\n"
           "  1 - Exact Match\n"
           "  2 - Before\n"
           "  3 - After\n"
           "  4 - Range\n");

    char *mode = prompt_validated_input(fp, "> mode: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!mode) return NULL;

    char *date_1 = prompt_validated_input(fp, "> date (YYYY-MM-DD): ", is_valid_date_format, "[TICKET] Bad date format. Must be YYYY-MM-DD.");
    if (!date_1) {
        free(mode);
        return NULL;
    }

    if (atoi(mode) == 4) {
        char *date_2 = prompt_validated_input(fp, "> second date (YYYY-MM-DD): ", is_valid_date_format, "[TICKET] Bad date format. Must be YYYY-MM-DD.");
        if (!date_2) {
            free(mode);
            free(date_1);
            return NULL;
        }

        char *content[] = {mode, date_1, date_2};
        return create_message(3, content);
    }

    char *content[] = {mode, date_1};
    return create_message(2, content);
}

message_t *input_save_tickets(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_write_file, "[LOAD] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}

message_t *input_load_tickets(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_read_file, "[LOAD] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}

message_t *input_quit(FILE *fp) {
    return NULL;
}