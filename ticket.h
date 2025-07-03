#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>

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
    uint32_t support_agent;    // uid
} ticket_t;

#endif /* TICKET_H */