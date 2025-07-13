#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "../../include/protocol.h"
#include "../../include/message.h"
#include "../../include/file-utils.h"

int send_request(int fd, request_t *req) {
    if (req == NULL) {
        fprintf(stderr, "%s(): NULL request.", __func__);
        return -1;
    }

    int code = htonl((int)req->code);
    if (write_all(fd, &code, sizeof(int)) != sizeof(int)) {
        fprintf(stderr, "%s(): write error.", __func__);
        return -1;
    }

    if (write_message(fd, req->payload) < 0) {
        fprintf(stderr, "%s(): write error.", __func__);
        return -1;
    }

    return 1;
}

int recv_request(int fd, request_t **req_out) {
    request_t *req = malloc(sizeof(request_t));
    if (!req) {
        perror("malloc");
        return -1;
    }

    int code_net;
    if (read_all(fd, &code_net, sizeof(int)) != sizeof(int)) {
        fprintf(stderr, "%s(): read failed.", __func__);
        free(req);
        return -1;
    }

    req->code = (RequestCode)ntohl(code_net);
    
    if (read_message(fd, &req->payload) < 0) {
        fprintf(stderr, "%s(): read failed.", __func__);
        free(req);
        return -1;
    }

    *req_out = req;
    return 1;
}

int send_response(int fd, response_t *resp) { 
    if (resp == NULL) {
        fprintf(stderr, "%s(): trying to send a null response.", __func__);
        return -1;
    }

    int code = htonl((int)resp->code);
    if (write_all(fd, &code, sizeof(int)) != sizeof(int)) {
        fprintf(stderr, "%s(): write failed.", __func__);
        return -1;
    }

    if (write_message(fd, resp->payload) < 0) {
        fprintf(stderr, "%s(): write message failed.", __func__);
        return -1;
    }

    return 1;
}

int recv_response(int fd, response_t **resp_out) {
    response_t *resp = malloc(sizeof(response_t));
    if (!resp) {
        perror("malloc");
        return -1;
    }

    int code_net;
    if (read_all(fd, &code_net, sizeof(int)) != sizeof(int)) {
        fprintf(stderr, "%s(): read failed.", __func__);
        free(resp);
        return -1;
    }

    resp->code = (ResponseCode)ntohl(code_net);
    
    if (read_message(fd, &resp->payload) < 0) {
        fprintf(stderr, "%s(): read failed.", __func__);
        free(resp);
        return -1;
    }

    *resp_out = resp;
    return 1;
}