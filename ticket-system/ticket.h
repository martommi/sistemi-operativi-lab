#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct user user_t;
typedef struct ticket ticket_t;

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

int open_ticket(char *title, char *desc, char *date, TicketPriority priority, TicketStatus status, user_t *support_agent);
int delete_ticket(ticket_t *ticket);
int count_tickets();
int assign_ticket(ticket_t *ticket, user_t *support_agent);
int update_status(ticket_t *ticket, TicketStatus new_status);

int get_all_tickets(ticket_t **destination);                           // wrapper for get_tickets with no filter (set as NULL)
int get_tickets_by_priority(ticket_t **dest, TicketPriority priority);
int get_tickets_by_status(ticket_t **dest, TicketStatus status);
int get_tickets_by_support_agent(ticket_t **dest, user_t *support_agent);
int get_tickets_by_title(ticket_t **dest, const char *query, TitleMatchMode mode);
int get_tickets_by_date(ticket_t **dest, DateMatchMode mode, const char *arg1, const char *arg2);

int save_tickets(const char *filename);
int load_tickets(const char *filename);

#endif /* TICKET_H */