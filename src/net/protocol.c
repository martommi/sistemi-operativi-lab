#include "../../include/protocol.h"
#include "../../include/message.h"
#include "../../include/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

void send_request(int fd, request_t *req) {
    int code = htonl((int)req->code);
    if (write_all(fd, &code, sizeof(int)) != sizeof(int)) {
        perror("write code");
        exit(EXIT_FAILURE);
    }
    write_message(fd, req->payload);
}

void recv_request(int fd, request_t **req_out) {
    request_t *req = malloc(sizeof(request_t));
    if (!req) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int code_net;
    if (read_all(fd, &code_net, sizeof(int)) != sizeof(int)) {
        perror("read code");
        free(req);
        exit(EXIT_FAILURE);
    }

    req->code = (RequestCode)ntohl(code_net);
    read_message(fd, &req->payload);
    *req_out = req;
}

void send_response(int fd, response_t *resp) {
    int code = htonl((int)resp->code);
    if (write_all(fd, &code, sizeof(int)) != sizeof(int)) {
        perror("write response code");
        exit(EXIT_FAILURE);
    }
    write_message(fd, resp->payload);
}

void recv_response(int fd, response_t **resp_out) {
    response_t *resp = malloc(sizeof(response_t));
    if (!resp) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int code_net;
    if (read_all(fd, &code_net, sizeof(int)) != sizeof(int)) {
        perror("read response code");
        free(resp);
        exit(EXIT_FAILURE);
    }

    resp->code = (ResponseCode)ntohl(code_net);
    read_message(fd, &resp->payload);
    *resp_out = resp;
}