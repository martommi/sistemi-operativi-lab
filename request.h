#ifndef REQUEST_H
#define REQUEST_H

/*
    Client side
*/

typedef struct request {
    char label;
    void (*request)(int fd);
} request_t;

// una serie di funzioni per interagire con la lista dei ticket

#endif /* REQUEST_H */