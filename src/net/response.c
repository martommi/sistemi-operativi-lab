#include "../../include/response.h"
#include "../../include/message.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int print_response(FILE *f, response_t *resp) {
    for (size_t i = 0; i < resp->payload->size; i++) {
        fwrite(resp->payload->content[i], sizeof(char), strlen(resp->payload->content[i]), f);
    }
    
    return 1;
}