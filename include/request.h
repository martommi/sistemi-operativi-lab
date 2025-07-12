#ifndef REQUEST_H
#define REQUEST_H

/*
    Client side
*/

#include "message.h"

typedef enum RequestCode {
    /* Authentication and user management */
    REQ_LOGIN = 1,
    REQ_LOGOUT,
    REQ_REGISTER_USER,
    REQ_REMOVE_USER,
    REQ_FIND_USER,
    REQ_SAVE_USERS,
    REQ_LOAD_USERS,
    /* Ticket management */
    REQ_OPEN_TICKET,
    REQ_DELETE_TICKET,
    REQ_ASSIGN_TICKET,
    REQ_UPDATE_TICKET_STATUS,
    REQ_GET_ALL_TICKETS,
    REQ_TICKET_FILTER_PRIORITY,
    REQ_TICKET_FILTER_STATUS,
    REQ_TICKET_FILTER_SUPPORT_AGENT,
    REQ_TICKET_FILTER_TITLE,
    REQ_TICKET_FILTER_DATE,
    REQ_SAVE_TICKETS,
    REQ_LOAD_TICKETS,
    /* Other */
    REQ_QUIT
} RequestCode;

typedef struct request {
    RequestCode code;
    message_t *payload;
} request_t;

request_t *create_request(RequestCode code, message_t *msg);
void free_request(request_t *req);

#endif /* REQUEST_H */