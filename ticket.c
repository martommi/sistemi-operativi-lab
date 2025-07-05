#include "./ticket.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static ticket_t *head = NULL;

int get_tickets(ticket_t **destination, ticket_filter filter, ...) {
    ticket_t *current = head;
    int count = 0;

    va_list args;
    va_start(args, filter);

    while (current) {
        va_list args_copy;
        va_copy(args_copy, args);

        if (filter(current, args_copy)) {
            destination[count++] = current;
        }

        va_end(args_copy);
        current = current->next;
    }

    va_end(args);
    return count;
}

int get_by_priority(ticket_t *target, va_list args) {
    TicketPriority priority = va_arg(args, TicketPriority);
    return (target->priority == priority);
}

int get_by_status(ticket_t *target, va_list args) {
    TicketStatus status = va_arg(args, TicketStatus);
    return (target->status == status);
}

int get_by_title(ticket_t *target, va_list args) {
    const char *query = va_arg(args, const char *);
    TitleMatchMode mode = va_arg(args, TitleMatchMode);

    switch (mode) {
        case TITLE_MATCH_EXACT:
            return strcmp(target->title, query) == 0;

        case TITLE_MATCH_CONTAINS:
            return strstr(target->title, query) != NULL;

        case TITLE_MATCH_STARTS_WITH:
            return strncmp(target->title, query, strlen(query)) == 0;

        default:
            return 0;
    }
}

int get_by_date(ticket_t *target, va_list args) {
    const char *ticket_date = target->date;
    if (!ticket_date) return 0;

    DateMatchMode mode = va_arg(args, DateMatchMode);

    switch (mode) {
        case DATE_MATCH_EXACT: {
            const char *target = va_arg(args, const char *);
            return strcmp(ticket_date, target) == 0;
        }

        case DATE_MATCH_BEFORE: {
            const char *target = va_arg(args, const char *);
            return strcmp(ticket_date, target) < 0;
        }

        case DATE_MATCH_AFTER: {
            const char *target = va_arg(args, const char *);
            return strcmp(ticket_date, target) > 0;
        }

        case DATE_MATCH_RANGE: {
            const char *start = va_arg(args, const char *);
            const char *end = va_arg(args, const char *);
            return strcmp(ticket_date, start) >= 0 && strcmp(ticket_date, end) <= 0;
        }

        default:
            return 0;
    }
}
