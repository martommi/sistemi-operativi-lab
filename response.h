#ifndef RESPONSE_H
#define RESPONSE_H

/*
    Server side
*/

typedef struct session session_t;
typedef struct message message_t;

typedef struct response {
    void (*response)(session_t *, message_t *);
} response_t;

// una lista di funzioni per interagire con la libreria dei ticket e restituire al client

#endif /* RESPONSE_H */