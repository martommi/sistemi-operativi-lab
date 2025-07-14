#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../include/server-dispatcher.h"
#include "../../include/server-tickets-dispatcher.h"
#include "../../include/server-users-dispatcher.h"
#include "../../include/user-lib.h"
#include "../../include/message.h"
#include "../../include/protocol.h"
#include "../../include/request.h"
#include "../../include/response.h"
#include "../../include/ticket.h"
#include "../../include/utils.h"

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

    if (resp == NULL) {
        fprintf(stderr, "%s(): null response.\n", __func__);
        return -1;
    }

    if (send_response(session->fd, resp) < 0) {
        fprintf(stderr, "%s(): error sending response.\n", __func__);
        return -1;
    }

    int keep_session_alive = !(resp->code == RESP_EXIT);
    free_response(&resp);
    return keep_session_alive;
}

response_t *handle_invalid_request() {
    return create_response(RESP_INVALID_REQUEST, create_message_from_str("[SERVER] Invalid code.\n"));
}

response_t *handle_quit(session_t *session, message_t *msg) {
    const char *code = generate_day_code();

    char user_path[256];
    char ticket_path[256];

    snprintf(user_path, sizeof(user_path), "users:%s_%s.dat", DEFAULT_USERS_PATH, code);
    snprintf(ticket_path, sizeof(ticket_path), "tickets:%s_%s.dat", DEFAULT_TICKETS_PATH, code);

    if (save_users(user_path) < 0 || save_tickets(ticket_path) < 0) {
        fprintf(stderr, "%s(): failed to save on file.\n", __func__);
        return create_response(RESP_ERROR, create_message_from_str("[SERVER] Error saving on file tickets and users (wrong path?).\n"));
    }

    free_message(&msg);

    return create_response(RESP_EXIT, create_message_from_str("[SERVER] Users and tickets saved. You will now be disconnected.\n"));
}

void init_test_users(void) {
    register_user("admin", "admin123", PRIVILEGES_ADMIN);
    register_user("agent", "agent123", PRIVILEGES_SUPPORT_AGENT);
}