#ifndef RESPONSE_H
#define RESPONSE_H

/*
    Server side
*/

#include "message.h"
#include <stdio.h>

typedef enum ResponseCode {
    RESP_ERROR = -1,
    RESP_EXIT,
    RESP_INVALID_REQUEST,
    RESP_OK
} ResponseCode;

typedef struct response {
    ResponseCode code;
    message_t *payload;
} response_t;

response_t *create_response(ResponseCode code, message_t *msg);
void free_response(response_t *response);
int print_response(FILE *f, response_t *resp);

#endif /* RESPONSE_H */