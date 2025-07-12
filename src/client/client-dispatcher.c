#include "../../include/client-dispatcher.h"
#include <ctype.h>
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


int is_number(const char *str) {
    if (*str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

int is_valid_path(const char *path, int flags) {
    return access(path, flags) == 0;    /* Checks if path exists and has permissions */
}

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
    char username[256];
    char password[256];
    char **content = malloc(2 * sizeof(char*));

    printf("> username: ");
    fflush(stdout);

    if (!fgets(username, sizeof(username), fp)) {
        free(content);
        return NULL;
    }

    username[strcspn(username, "\n")] = '\0';

    printf("> password: ");
    fflush(stdout);

    if (!fgets(password, sizeof(password), fp)) {
        free(content);
        return NULL;
    }

    password[strcspn(password, "\n")] = '\0';

    content[0] = username;
    content[1] = password;

    return create_message(2, content);
}

message_t *input_logout(FILE *fp) {
    return NULL;    /* No input required */
}

message_t *input_register_user(FILE *fp) {
    char username[256];
    char password[256];
    char **content = malloc(2 * sizeof(char*));

    printf("> choose a username: ");
    fflush(stdout);

    if (!fgets(username, sizeof(username), fp)) {
        free(content);
        return NULL;
    }

    username[strcspn(username, "\n")] = '\0';

    printf("> choose a password: ");
    fflush(stdout);

    if (!fgets(password, sizeof(password), fp)) {
        free(content);
        return NULL;
    }

    password[strcspn(password, "\n")] = '\0';

    content[0] = username;
    content[1] = password;

    return create_message(2, content);
}

message_t *input_remove_user(FILE *fp) {
    char uid[16];
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;

    do {
        printf("> uid of user to delete: ");
        fflush(stdout);

        if (!fgets(uid, sizeof(uid), fp)) {
            free(content);
            return NULL;
        }

        uid[strcspn(uid, "\n")] = '\0';

        if (!is_number(uid)) {
            fprintf(stderr, "[REMOVE] Invalid UID. Must be a number.\n");
        }

    } while (!is_number(uid));

    content[0] = strdup(uid);
    if (!content[0]) {
        free(content);
        return NULL;
    }

    return create_message(1, content);
}

message_t *input_find_user(FILE *fp) {
    char username[256];
    char limit[16];
    char **content = malloc(2 * sizeof(char *));

    printf("> find username: ");
    fflush(stdout);

    if (!fgets(username, sizeof(username), fp)) {
        free(content);
        return NULL;
    }

    content[0] = username;

    do {
        printf("> limit (default = 1): ");
        fflush(stdout);

        if (!fgets(limit, sizeof(limit), fp)) {
            free(content);
            return NULL;
        }

        limit[strcspn(limit, "\n")] = '\0';

        if (!is_number(limit)) {
            printf("[FIND] Invalid input. Must be a number.\n");
        }

    } while (!is_number(limit));

    content[1] = strdup(limit);
    return create_message(2, content);
}

message_t *input_save_users(FILE *fp) {
    char path[256];
    char **content = malloc(sizeof(char *));
    
    int condition = 0;
    do {
        printf("> file path: ");
        fflush(stdout);

        if (!fgets(path, sizeof(path), fp)) {
            free(content);
            return NULL;
        }

        path[strcspn(path, "\n")] = '\0';

        condition = is_valid_path(path, F_OK | W_OK);
        if (!condition) {
            printf("[SAVE] Invalid path. Please provide an existing path with write permission.\n");
        }

    } while (!condition);

    content[0] = strdup(path);
    return create_message(1, content);
}

message_t *input_load_users(FILE *fp) {
    char path[256];
    char **content = malloc(sizeof(char *));
    
    int condition = 0;
    do {
        printf("> file path: ");
        fflush(stdout);

        if (!fgets(path, sizeof(path), fp)) {
            free(content);
            return NULL;
        }

        path[strcspn(path, "\n")] = '\0';

        condition = is_valid_path(path, F_OK | R_OK);
        if (!condition) {
            printf("[LOAD] Invalid path. Please provide an existing path with read permission.\n");
        }

    } while (!condition);

    content[0] = strdup(path);
    return create_message(1, content);
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
    char path[256];
    char **content = malloc(sizeof(char *));
    
    int condition = 0;
    do {
        printf("> file path: ");
        fflush(stdout);

        if (!fgets(path, sizeof(path), fp)) {
            free(content);
            return NULL;
        }

        path[strcspn(path, "\n")] = '\0';

        condition = is_valid_path(path, F_OK | W_OK);
        if (!condition) {
            printf("[SAVE] Invalid path. Please provide an existing path with write permission.\n");
        }

    } while (!condition);

    content[0] = strdup(path);
    return create_message(1, content);
}

message_t *input_load_tickets(FILE *fp) {
    char path[256];
    char **content = malloc(sizeof(char *));
    
    int condition = 0;
    do {
        printf("> file path: ");
        fflush(stdout);

        if (!fgets(path, sizeof(path), fp)) {
            free(content);
            return NULL;
        }

        path[strcspn(path, "\n")] = '\0';

        condition = is_valid_path(path, F_OK | R_OK);
        if (!condition) {
            printf("[LOAD] Invalid path. Please provide an existing path with read permission.\n");
        }

    } while (!condition);

    content[0] = strdup(path);
    return create_message(1, content);
}

message_t *input_quit(FILE *fp) {
    return NULL;
}