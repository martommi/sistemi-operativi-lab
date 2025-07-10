#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "request.h"
#include "response.h"

void send_request(int fd, request_t *req);
void recv_request(int fd, request_t **req);

void send_response(int fd, response_t *resp);
void recv_response(int fd, response_t **resp);

#endif /* PROTOCOL_H */