#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>

#include "./user.h"

typedef enum TicketStatus {
    TICKET_STATUS_OPEN,
    TICKET_STATUS_ONGOING,
    TICKET_STATUS_CLOSED
} TicketStatus;

typedef struct ticket {
    char *title;
    char *description;
    char *date;
    uint16_t priority;
    TicketStatus status;
    struct user *support_agent;
} ticket_t;

#endif /* TICKET_H */