#include "../../include/protocol.h"
#include "../../include/message.h"
#include <unistd.h>
#include <stdlib.h>

void send_request(int fd, request_t *req) {
    int code = (int)req->code;
    write(fd, &code, sizeof(int));
    write_message(fd, req->payload);
}

void recv_request(int fd, request_t **req_out) {
    request_t *req = malloc(sizeof(request_t));
    int code;
    read(fd, &code, sizeof(int));
    req->code = (RequestCode)code;
    read_message(fd, &req->payload);
    *req_out = req;
}

void send_response(int fd, response_t *resp) {
    int code = (int)resp->code;
    write(fd, &code, sizeof(int));
    write_message(fd, resp->payload);
}

void recv_response(int fd, response_t **resp_out) {
    response_t *resp = malloc(sizeof(response_t));
    int code;
    read(fd, &code, sizeof(int));
    resp->code = (ResponseCode)code;
    read_message(fd, &resp->payload);
    *resp_out = resp;
}
