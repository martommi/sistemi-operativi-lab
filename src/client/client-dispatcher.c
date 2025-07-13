#include "../../include/client-dispatcher.h"
#include "../../include/client-tickets-dispatcher.h"
#include "../../include/client-users-dispatcher.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DISPATCH_TABLE_SIZE (sizeof(dispatch_table) / sizeof(dispatcher_t))

static dispatcher_t dispatch_table[] = {
    { REQ_LOGIN,                     input_login },
    { REQ_LOGOUT,                    input_logout },
    { REQ_REGISTER_USER,             input_register_user },
    { REQ_REMOVE_USER,               input_remove_user },
    { REQ_FIND_USER,                 input_find_user },
    { REQ_SAVE_USERS,                input_save_users },
    { REQ_LOAD_USERS,                input_load_users },

    { REQ_OPEN_TICKET,               input_open_ticket },
    { REQ_DELETE_TICKET,             input_delete_ticket },
    { REQ_ASSIGN_TICKET,             input_assign_ticket },
    { REQ_UPDATE_TICKET_STATUS,      input_update_ticket_status },
    { REQ_GET_ALL_TICKETS,           input_get_all_tickets },
    { REQ_TICKET_FILTER_PRIORITY,    input_ticket_filter_priority },
    { REQ_TICKET_FILTER_STATUS,      input_ticket_filter_status },
    { REQ_TICKET_FILTER_SUPPORT_AGENT, input_ticket_filter_support_agent },
    { REQ_TICKET_FILTER_TITLE,       input_ticket_filter_title },
    { REQ_TICKET_FILTER_DATE,        input_ticket_filter_date },
    { REQ_SAVE_TICKETS,              input_save_tickets },
    { REQ_LOAD_TICKETS,              input_load_tickets },

    { REQ_QUIT,                      input_quit }
};

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

    if (fscanf(fp, "%d", &choice) != 1) {
        fprintf(stderr, "[CLIENT] Invalid input.\n");
        *req_ptr = NULL;
        return -1;
    }

    int c;
    while ((c = fgetc(fp)) != '\n' && c != EOF);    /* Flush newline */

    input_handler handler_fn = NULL;
    for (size_t i = 0; i < sizeof(dispatch_table)/sizeof(dispatch_table[0]); i++) {
        if (dispatch_table[i].code == choice) {
            handler_fn = dispatch_table[i].fn;
            break;
        }
    }

    if (handler_fn == NULL) {
        fprintf(stderr, "[CLIENT] Invalid or unknown request code: %d\n", choice);
        *req_ptr = NULL;
        return -1;
    }

    message_t *msg = handler_fn(fp);

    *req_ptr = create_request((RequestCode)choice, msg);
    return 1;
}

message_t *input_quit(FILE *fp) {
    return NULL;
}