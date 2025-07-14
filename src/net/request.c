#include <stdio.h>
#include <stdlib.h>

#include "../../include/request.h"

request_t *create_request(RequestCode code, message_t *msg) {
    request_t *req = malloc(sizeof(request_t));
    if (!req) {
        perror("malloc()");
        return NULL;
    }

    req->code = code;
    req->payload = msg;    /* Nullable */
    return req;
}

void free_request(request_t **req) {
    if (*req == NULL) return;
    free_message(&(*req)->payload);
    free(*req);
}
