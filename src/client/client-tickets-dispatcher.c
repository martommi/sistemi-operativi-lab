#include "../../include/client-tickets-dispatcher.h"
#include "../../include/utils.h"
#include <stdlib.h>

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
    printf("Filter by priority: \n\
        1- Low\n\
        2- Standard\n\
        3- High\n\
        ");
    char *priority = prompt_validated_input(fp, "> filter priority: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!priority) return NULL;
    char *content[] = { priority };
    return create_message(2, content);
}

message_t *input_ticket_filter_status(FILE *fp) {
    printf("Filter by status: \n\
        1- Open\n\
        2- Ongoing\n\
        3- Closed\n\
        ");
    char *status = prompt_validated_input(fp, "> filter status: ", is_number, "[TICKET] Invalid input. Must be a number.");
    if (!status) return NULL;
    char *content[] = { status };
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