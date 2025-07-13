#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/server-dispatcher.h"
#include "../../include/server-tickets-dispatcher.h"
#include "../../include/server-users-dispatcher.h"
#include "../../include/user-lib.h"
#include "../../include/message.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/ticket.h"

#define DEFAULT_USERS_PATH "../../data/users/"
#define DEFAULT_TICKETS_PATH "../../data/tickets/"
#define DISPATCH_TABLE_SIZE (sizeof(dispatch_table) / sizeof(dispatcher_t))

static dispatcher_t dispatch_table[] = {
    { REQ_LOGIN,                     handle_login },
    { REQ_LOGOUT,                    handle_logout },
    { REQ_REGISTER_USER,             handle_register_user },
    { REQ_REMOVE_USER,               handle_remove_user },
    { REQ_FIND_USER,                 handle_find_users },
    { REQ_SAVE_USERS,                handle_save_users },
    { REQ_LOAD_USERS,                handle_load_users },

    { REQ_OPEN_TICKET,               handle_open_ticket },
    { REQ_DELETE_TICKET,             handle_delete_ticket },
    { REQ_ASSIGN_TICKET,             handle_assign_ticket },
    { REQ_UPDATE_TICKET_STATUS,      handle_update_ticket_status },
    { REQ_GET_ALL_TICKETS,           handle_get_all_tickets },
    { REQ_TICKET_FILTER_PRIORITY,    handle_filter_by_priority }, 
    { REQ_TICKET_FILTER_STATUS,      handle_filter_by_status },
    { REQ_TICKET_FILTER_SUPPORT_AGENT, handle_filter_by_support_agent },
    { REQ_TICKET_FILTER_TITLE,       handle_filter_by_title },
    { REQ_TICKET_FILTER_DATE,        handle_filter_by_date },
    { REQ_SAVE_TICKETS,              handle_save_tickets },
    { REQ_LOAD_TICKETS,              handle_load_tickets },

    { REQ_QUIT,                      handle_quit }
};

int handle_request(session_t *session, request_t *req) {
    response_t *resp = NULL;
    handler fn = NULL;

    for (size_t i = 0; i < DISPATCH_TABLE_SIZE; i++) {
        if (dispatch_table[i].code == req->code) {
            fn = dispatch_table[i].fn;
            break;
        }
    }

    if (fn == NULL) {
        resp = handle_invalid_request();
    } else {
        resp = fn(session, req->payload);
    }

    send_response(session->fd, resp);

    int keep_session_alive = (resp->code == RESP_EXIT);
    free_response(resp);
    return keep_session_alive;
}

response_t *handle_invalid_request() {
    return create_response(RESP_INVALID_REQUEST, create_message_from_str("[SERVER] Invalid code."));
}

response_t *handle_quit(session_t *session, message_t *msg) {
    //generate day code
    char *name = "";
    save_users(strcat(DEFAULT_USERS_PATH, name));
    save_tickets(strcat(DEFAULT_TICKETS_PATH, name));

    free_message(msg);

    return create_response(RESP_EXIT, create_message_from_str("[SERVER] Users and tickets saved. You will now be disconnected."));
}