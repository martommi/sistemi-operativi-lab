#ifndef CLIENT_DISPATCHER_H
#define CLIENT_DISPATCHER_H

#include "request.h"
#include <stdio.h>

typedef message_t *(*input_handler)(FILE *f);

typedef struct dispatcher {
    int code;
    input_handler fn;
} dispatcher_t;

void print_menu();
int compose_request(FILE *fp, request_t **req);
message_t *input_quit(FILE *fp);

#endif