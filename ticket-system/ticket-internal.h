#include "ticket.h"

typedef struct ticket {
    char *title;
    char *description;
    char *date;                // YYYY-MM-DD
    TicketPriority priority;
    TicketStatus status;
    struct user *support_agent;
    struct ticket *next;
} ticket_t;

typedef int (*ticket_filter)(const ticket_t *target, va_list args);

ticket_t *_create_ticket(char *title, char *desc, char *date, TicketPriority priority, TicketStatus status, user_t *support_agent);
void _free_ticket(ticket_t *target);

int _get_tickets(ticket_t **destination, ticket_filter filter, ...);
int get_by_priority(const ticket_t *target, va_list args);
int get_by_status(const ticket_t *target, va_list args);
int get_by_support_agent(const ticket_t *target, va_list args);
int get_by_title(const ticket_t *target, va_list args);
int get_by_date(const ticket_t *target, va_list args);

int _add_ticket(ticket_t *ticket);
int _remove_ticket(ticket_t *ticket);
int _count_tickets();
int _set_support_agent(ticket_t *ticket, user_t *support_agent);
int _set_status(ticket_t *ticket, TicketStatus new_status);