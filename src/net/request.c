#include "../../include/request.h"
#include <stdio.h>
#include <stdlib.h>

request_t *create_request(RequestCode code, message_t *msg) {
    request_t *req;
    if (!(req = (request_t *)malloc(sizeof(request_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    req->code = code;
    req->payload = msg;
    return req;
}

void free_request(request_t *response) {
    free_message(response->payload);
    free(response);
}