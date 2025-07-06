#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct user user_t;

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
    struct user *support_agent;
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

typedef int (*ticket_filter)(const ticket_t *target, va_list args);

ticket_t *create_ticket(char *title, char *desc, char *date, TicketPriority priority, TicketStatus status, user_t *support_agent);
void free_ticket(ticket_t *target);

// TODO
void add_ticket(ticket_t *ticket);
int remove_ticket(ticket_t *ticket);
int count_tickets();
int assign_ticket(ticket_t *ticket, user_t *support_agent);
int update_status(ticket_t *ticket, TicketStatus new_status);

int get_tickets(ticket_t **destination, ticket_filter filter, ...);
int get_all_tickets(ticket_t **destination);                           // wrapper for get_tickets with no filter (set as NULL)
int get_by_priority(const ticket_t *target, va_list args);
int get_by_status(const ticket_t *target, va_list args);
int get_by_support_agent(const ticket_t *target, va_list args);
int get_by_title(const ticket_t *target, va_list args);
int get_by_date(const ticket_t *target, va_list args);

#endif /* TICKET_H */