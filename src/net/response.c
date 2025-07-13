#include "../../include/response.h"
#include "../../include/message.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/response.h"
#include "../../include/message.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

response_t *create_response(ResponseCode code, message_t *message) {
    response_t *resp = malloc(sizeof(response_t));
    if (!resp) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    resp->code = code;
    resp->payload = message;
    return resp;
}

void free_response(response_t *response) {
    if (!response) return;
    free_message(response->payload);
    free(response);
}

int print_response(FILE *f, response_t *resp) {
    if (!f || !resp || !resp->payload || !resp->payload->content)
        return 0;

    for (size_t i = 0; i < resp->payload->size; i++) {
        const char *line = resp->payload->content[i];
        if (line) {
            if (fwrite(line, sizeof(char), strlen(line), f) < strlen(line)) {
                perror("fwrite()");
                return 0;
            }
        }
    }

    return 1;
}