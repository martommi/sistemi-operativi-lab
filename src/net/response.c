#include "../../include/response.h"
#include "../../include/message.h"
#include <stdio.h>
#include <stdlib.h>

response_t *create_response(ResponseCode code, message_t *message) {
    response_t *resp;
    if (!(resp = (response_t *)malloc(sizeof(response_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    resp->code = code;
    resp->payload = message;
    return resp;
}

void free_response(response_t *response) {
    free_message(response->payload);
    free(response);
}