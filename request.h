#ifndef REQUEST_H
#define REQUEST_H

/*
    Client side
*/

#include "message.h"

typedef enum RequestCode {
    REQ_LOGIN,
    REQ_LOGOUT,
    REQ_GET_ALL_TICKETS,
    REQ_GET_TICKETS_BY_PRIORITY,
    REQ_CREATE_TICKET,
    REQ_UPDATE_STATUS,
    // altri...
} RequestCode;

typedef struct request {
    RequestCode code;
    message_t *payload;
} request_t;

// una serie di funzioni per interagire con la lista dei ticket

#endif /* REQUEST_H */