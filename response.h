#ifndef RESPONSE_H
#define RESPONSE_H

/*
    Server side
*/

#include "message.h"

typedef enum ResponseCode {
    RESP_OK,
    RESP_ERROR,
    RESP_AUTH_FAILED,
    RESP_INVALID_REQUEST,
    RESP_TICKET_LIST,
    RESP_ACK,
    // altri...
} ResponseCode;

typedef struct response {
    ResponseCode code;
    // void (*response)(session_t *, message_t *);
    message_t *payload;
} response_t;

// una lista di funzioni per interagire con la libreria dei ticket e restituire al client

#endif /* RESPONSE_H */