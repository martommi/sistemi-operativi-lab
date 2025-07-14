#include <stdlib.h>

#include "../../include/client-tickets-dispatcher.h"

#include <string.h>

#include "../../include/utils.h"

message_t *input_open_ticket(FILE *fp) {
    char *title = prompt_string(fp, "> ticket title: ");
    char *description = prompt_string(fp, "> ticket description: ");
    char *date = prompt_validated_input(fp, "> ticket date (default = today): ", is_valid_date_format, "[TICKET] invalid date format. Must be YYYY-MM-DD.\n");
    if (!title || !description || !date) return NULL;
    char **content = malloc(3 * sizeof(char*));
    if (!content) return NULL;
    if (date[0] == '\0') date = strdup((char *)get_today());
    content[0] = title;
    content[1] = description;
    content[2] = date;
    return create_message(3, content);
}

message_t *input_delete_ticket(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[DELETE] Invalid ticket ID.\n");
    if (!tid) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = tid;
    return create_message(1, content);
}

message_t *input_assign_ticket(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[ASSIGN] Invalid ticket ID.\n");
    char *agent = prompt_string(fp, "> assign to (agent username): ");
    if (!tid || !agent) return NULL;
    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = tid;
    content[1] = agent;
    return create_message(2, content);
}

message_t *input_update_ticket_status(FILE *fp) {
    char *tid = prompt_validated_input(fp, "> ticket ID: ", is_number, "[UPDATE] Invalid ticket ID.\n");
    printf("Select new status: \n\
        1- Open\n\
        2- Ongoing\n\
        3- Closed\n\
        \n");
    char *status = prompt_validated_input(fp, "> new status: ", is_number, "[TICKET] Invalid input. Must be a number.\n");
    if (!tid || !status) return NULL;
    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = tid;
    content[1] = status;
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
        \n");
    char *priority = prompt_validated_input(fp, "> filter priority: ", is_number, "[TICKET] Invalid input. Must be a number.\n");
    if (!priority) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = priority;
    return create_message(1, content);
}

message_t *input_ticket_filter_status(FILE *fp) {
    printf("Filter by status: \n\
        1- Open\n\
        2- Ongoing\n\
        3- Closed\n\
        \n");
    char *status = prompt_validated_input(fp, "> filter status: ", is_number, "[TICKET] Invalid input. Must be a number.\n");
    if (!status) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = status;
    return create_message(1, content);
}

message_t *input_ticket_filter_support_agent(FILE *fp) {
    char *agent = prompt_string(fp, "> support agent (username): ");
    if (!agent) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = agent;
    return create_message(1, content);
}

message_t *input_ticket_filter_title(FILE *fp) {
    printf("Tile matching mode:\n"
        "  1 - Exact Match\n"
        "  2 - Contains\n"
        "  3 - Starts With\n");

    char *mode = prompt_validated_input(fp, "> mode: ", is_number, "[TICKET] Invalid input. Must be a number.\n");
    char *title = prompt_string(fp, "> ticket title: ");
    if (!title || !mode) return NULL;
    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = title;
    content[1] = mode;
    return create_message(2, content);
}

message_t *input_ticket_filter_date(FILE *fp) {
    printf("Date matching mode:\n"
           "  1 - Exact Match\n"
           "  2 - Before\n"
           "  3 - After\n"
           "  4 - Range\n");

    char *mode = prompt_validated_input(fp, "> mode: ", is_number, "[TICKET] Invalid input. Must be a number.\n");
    if (!mode) return NULL;

    char *date_1 = prompt_validated_input(fp, "> date (YYYY-MM-DD): ", is_valid_date_format, "[TICKET] Bad date format. Must be YYYY-MM-DD.\n");
    if (!date_1) {
        free(mode);
        return NULL;
    }

    if (atoi(mode) == 4) {
        char *date_2 = prompt_validated_input(fp, "> second date (YYYY-MM-DD): ", is_valid_date_format, "[TICKET] Bad date format. Must be YYYY-MM-DD.\n");
        if (!date_2) {
            free(mode);
            free(date_1);
            return NULL;
        }

        char **content = malloc(3 * sizeof(char *));
        if (!content) return NULL;
        content[0] = mode;
        content[1] = date_1;
        content[2] = date_2;
        return create_message(3, content);
    }

    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = mode;
    content[1] = date_1;
    return create_message(2, content);
}

message_t *input_save_tickets(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_write_file, "[LOAD] Invalid path.\n");
    if (!path) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = path;
    return create_message(1, content);
}

message_t *input_load_tickets(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_read_file, "[LOAD] Invalid path.\n");
    if (!path) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = path;
    return create_message(1, content);
}