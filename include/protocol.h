#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "request.h"
#include "response.h"

int send_request(int fd, request_t *req);
int recv_request(int fd, request_t **req);

int send_response(int fd, response_t *resp);
int recv_response(int fd, response_t **resp);

#endif /* PROTOCOL_H */