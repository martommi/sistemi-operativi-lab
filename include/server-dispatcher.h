#ifndef SERVER_DISPATCHER_H
#define SERVER_DISPATCHER_H

#include "session.h"
#include "message.h"
#include "request.h"
#include "response.h"

typedef response_t *(*handler)(session_t *, message_t *);

typedef struct dispatcher {
    int code;
    handler fn;
} dispatcher_t;

int handle_request(session_t *session, request_t *req);
response_t *handle_invalid_request();
response_t *handle_quit(session_t *session, message_t *msg);
void init_test_users();

#endif /* SERVER_DISPATCHER_H */