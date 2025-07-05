#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum TicketStatus {
    TICKET_STATUS_OPEN,
    TICKET_STATUS_ONGOING,
    TICKET_STATUS_CLOSED
} TicketStatus;

typedef enum TicketPriority {
    TICKET_PRIORITY_LOW,
    TICKET_PRIORITY_STANDARD,
    TICKET_PRIORITY_HIGH
} TicketPriority;

typedef struct ticket {
    char *title;
    char *description;
    char *date;                // YYYY-MM-DD
    TicketPriority priority;
    TicketStatus status;
    uint32_t support_agent;    // uid (not sure about this, thinking of referencing user struct) TODO
    struct ticket *next;
} ticket_t;

typedef enum TitleMatchMode {
    TITLE_MATCH_EXACT,
    TITLE_MATCH_CONTAINS,
    TITLE_MATCH_STARTS_WITH
} TitleMatchMode;

typedef enum DateMatchMode {
    DATE_MATCH_EXACT,
    DATE_MATCH_BEFORE,
    DATE_MATCH_AFTER,
    DATE_MATCH_RANGE
} DateMatchMode;

int assign_ticket(const ticket_t *ticket, uint32_t support_agent_uid);
int update_status(const ticket_t *ticket, TicketStatus new_status);
int update_support_agent(const ticket_t *target, uint32_t new_support_agent);

typedef int (*ticket_filter)(ticket_t *target, va_list args);
int get_tickets(ticket_t **destination, ticket_filter filter, ...);

int get_by_priority(ticket_t *target, va_list args);
int get_by_status(ticket_t *target, va_list args);
int get_by_support_agent(ticket_t *target, va_list args);
int get_by_title(ticket_t *target, va_list args);
int get_by_date(ticket_t *target, va_list args);

#endif /* TICKET_H */