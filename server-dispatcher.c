#include <stdlib.h>

#include "server-dispatcher.h"
#include "authentication/session.h"
#include "authentication/user-lib.h"
#include "message.h"
#include "protocol.h"
#include "request.h"
#include "response.h"

typedef response_t *(*handler)(session_t *, message_t *);

static handler handlers[] = {
    [REQ_LOGIN]                      = handle_login,
    [REQ_LOGOUT]                     = handle_logout,
    [REQ_REGISTER_USER]             = handle_register_user,
    [REQ_REMOVE_USER]               = handle_remove_user,
    [REQ_FIND_USER]                 = handle_find_users,
    [REQ_SAVE_USERS]                = handle_save_users,
    [REQ_LOAD_USERS]                = handle_load_users,

    [REQ_OPEN_TICKET]               = handle_create_ticket,
    [REQ_DELETE_TICKET]             = NULL, // handle_delete_ticket da implementare
    [REQ_ASSIGN_TICKET]             = NULL, // handle_assign_ticket da implementare
    [REQ_UPDATE_TICKET_STATUS]      = NULL, // handle_update_ticket_status da implementare
    [REQ_GET_ALL_TICKETS]           = handle_get_all_tickets,

    [REQ_TICKET_FILTER_PRIORITY]    = NULL, // handle_filter_by_priority da implementare
    [REQ_TICKET_FILTER_STATUS]      = NULL, // handle_filter_by_status da implementare
    [REQ_TICKET_FILTER_SUPPORT_AGENT] = NULL, // handle_filter_by_support_agent da implementare
    [REQ_TICKET_FILTER_TITLE]       = NULL, // handle_filter_by_title da implementare
    [REQ_TICKET_FILTER_DATE]        = NULL, // handle_filter_by_date da implementare

    [REQ_SAVE_TICKETS]              = NULL, // handle_save_tickets da implementare
    [REQ_LOAD_TICKETS]              = NULL  // handle_load_tickets da implementare
};

void handle_request(session_t *session, request_t *req) {
    response_t *resp = NULL;

    size_t map_size = sizeof(handlers) / sizeof(handlers[0]);

    if ((size_t)req->code >= map_size || handlers[req->code] == NULL) {
        resp = handle_invalid_request();
    } else {
        resp = handlers[req->code](session, req->payload);
    }

    send_response(session->fd, resp);
    free_response(resp);
}

response_t *handle_invalid_request() {
    return create_response(RESP_INVALID_REQUEST, create_message_from_str("Invalide code"));
}

response_t *handle_login(session_t *session, message_t *msg) {
    //controllare argomenti message e se già loggato
    return login(session, msg->content[0], msg->content[1])
        ? create_response(RESP_OK, create_message_from_str("Login successful"))
        : create_response(RESP_AUTH_FAILED, create_message_from_str("Login error"));
}

response_t *handle_logout(session_t *session, message_t *msg) {
    if (!is_logged_in(session)) return create_response(RESP_ACK, create_message_from_str("No user is logged in."));
    logout(session);
    return create_response(RESP_OK, create_message_from_str("Logout ok"));
}

response_t *handle_register_user(session_t *session, message_t *msg) {
    return register_user(msg->content[0], msg->content[1], session->privileges)
        ? create_response(RESP_OK, NULL)
        : create_response(RESP_ERROR, NULL);
}

response_t *handle_remove_user(session_t *session, message_t *msg) {
    return remove_user(atoi(msg->content[0]))
        ? create_response(RESP_OK, NULL)
        : create_response(RESP_ERROR, NULL);
}

response_t *handle_find_users(session_t *session, message_t *msg) {
    user_t **found;
    message_t *out;
    response_t *result;
    int count;

    if ((count = find_users(msg->content[0], atoi(msg->content[1]), found)) <= 0) { 
        return create_response(RESP_ERROR, out);
    }

    char **content = malloc(sizeof(char *) * count);

    for (int i = 0; i < count; i++) {
        // print to string in content
    }

    out = create_message(count, content);
    result = create_response(RESP_OK, out);
    return result;
}

response_t *handle_save_users(session_t *session, message_t *msg) {
    return save_users(msg->content[0])
        ? create_response(RESP_OK, NULL)
        : create_response(RESP_ERROR, NULL);
}

response_t *handle_load_users(session_t *session, message_t *msg) {
    return load_users(msg->content[0])
        ? create_response(RESP_OK, NULL)
        : create_response(RESP_ERROR, NULL);
}

