#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticket.h"
#include "ticket-internal.h"

int open_ticket(char *title, char *desc, char *date, TicketPriority priority, TicketStatus status, user_t *support_agent) {
    ticket_t *ticket = _create_ticket(title, desc, date, priority, status, support_agent);

    if (_add_ticket(ticket)) return 1;

    fprintf(stderr, "%s(): failed adding new ticket (already exists?)", __func__);
    _free_ticket(ticket);
    return 0;
}

int delete_ticket(ticket_t *ticket) {
    if (!ticket) {
        fprintf(stderr, "%s(): NULL pointer.", __func__);
        return -1;
    }

    return _remove_ticket(ticket);
}

int count_tickets() {
    return _count_tickets();
}

int assign_ticket(ticket_t *ticket, user_t *support_agent) {
    return _set_support_agent(ticket, support_agent);
}

int update_status(ticket_t *ticket, TicketStatus new_status) {
    return _set_status(ticket, new_status);
}

int get_all_tickets(ticket_t **destination) {
    return _get_tickets(destination, NULL);
}

int get_tickets_by_priority(ticket_t **dest, TicketPriority priority) {
    return _get_tickets(dest, get_by_priority, priority);
}

int get_tickets_by_status(ticket_t **dest, TicketStatus status) {
    return _get_tickets(dest, get_by_status, status);
}

int get_tickets_by_support_agent(ticket_t **dest, user_t *support_agent) {
    return _get_tickets(dest, get_by_support_agent, support_agent);
}

int get_tickets_by_title(ticket_t **dest, const char *query, TitleMatchMode mode) {
    return _get_tickets(dest, get_by_title, query, mode);
}

int get_tickets_by_date(ticket_t **dest, DateMatchMode mode, const char *arg1, const char *arg2) {
    return _get_tickets(dest, get_by_date, arg1, arg2);
}

int get_tickets_by_tid(ticket_t **dest, uint32_t tid) {
    return _get_tickets(dest, get_by_tid, tid);
}

char *ticket_to_str(const ticket_t *ticket) {
    return _print_ticket(ticket);
}

int save_tickets(const char *filename) {
    return _save_tickets(filename);
}

int load_tickets(const char *filename) {
    return _load_tickets(filename);
}

