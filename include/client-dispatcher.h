#ifndef CLIENT_DISPATCHER_H
#define CLIENT_DISPATCHER_H

#include "request.h"
#include <stdio.h>

void print_menu(FILE *f);
int compose_request(FILE *f, request_t **r);

#endif