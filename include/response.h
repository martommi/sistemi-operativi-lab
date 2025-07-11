#ifndef RESPONSE_H
#define RESPONSE_H

/*
    Server side
*/

#include "message.h"

typedef enum ResponseCode {
    RESP_OK,
    RESP_ERROR,
    RESP_INVALID_REQUEST,
    RESP_EXIT
    // altri...
} ResponseCode;

typedef struct response {
    ResponseCode code;
    message_t *payload;
} response_t;

response_t *create_response(ResponseCode code, message_t *msg);
void free_response(response_t *response);

#endif /* RESPONSE_H */