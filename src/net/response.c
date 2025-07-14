#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/response.h"
#include "../../include/message.h"
#include "../../include/file-utils.h"

response_t *create_response(ResponseCode code, message_t *message) {
    response_t *resp = malloc(sizeof(response_t));
    if (!resp) {
        perror("malloc()");
        return NULL;
    }

    resp->code = code;

    if (message == NULL) {
        fprintf(stderr, "%s(): cannot assign a null message.\n", __func__);
        free(resp);
        return NULL;
    }

    resp->payload = message;
    return resp;
}

void free_response(response_t **response) {
    if (response == NULL || *response == NULL) return;

    free_message(&(*response)->payload);
    free(*response);
    *response = NULL;
}

int print_response(FILE *f, response_t *resp) {
    if (!f || !resp || !resp->payload || !resp->payload->content) {
        fprintf(stderr, "%s(): trying to print null response or with null args.\n", __func__);
        return -1;
    }

    for (size_t i = 0; i < resp->payload->size; i++) {
        const char *line = resp->payload->content[i];
        if (line) {
            if (write_all(fileno(f), line, strlen(line)) != strlen(line)) {
                fprintf(stderr, "%s(): write failed.\n", __func__);
                return -1;
            }
        }
    }

    return 1;
}