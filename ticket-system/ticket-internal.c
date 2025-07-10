#include "ticket-internal.h"
#include "../authentication/user-internal.h"
#include "ticket.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static ticket_t *head = NULL;
static uint32_t tid = 1;

ticket_t *_create_ticket(char *title, char *desc, char *date, TicketPriority priority, TicketStatus status, user_t *support_agent) {
    if (!title && !desc) {
        printf("create_ticket(): Title and Description are required fields, please provide them.");
        exit(EXIT_FAILURE);
    }

    ticket_t *ticket;
    if (!(ticket = (ticket_t *)malloc(sizeof(ticket_t)))) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    memset(ticket, 0, sizeof(ticket_t));

    ticket->tid = tid++;
    ticket->title = title;
    ticket->description = desc;
    ticket->date = date;
    ticket->priority = priority;
    ticket->status = status;
    ticket->support_agent = support_agent;
    ticket->next = NULL;

    return ticket;
}

void _free_ticket(ticket_t *target) {
    if (!target) {
        printf("free_ticket(): received NULL pointer.");
        exit(EXIT_FAILURE);
    }

    free(target->title);
    free(target->description);
    free(target->date);
    free(target);
}

int _get_tickets(ticket_t **destination, ticket_filter filter, ...) {
    ticket_t *current = head;
    int count = 0;

    va_list args;
    va_start(args, filter);

    while (current) {
        int match = 1;

        if (filter != NULL) {
            va_list args_copy;
            va_copy(args_copy, args);
            match = filter(current, args_copy);
            va_end(args_copy);
        }

        if (match) {
            destination[count++] = current;
        }

        current = current->next;
    }

    va_end(args);
    return count;
}

int get_by_priority(const ticket_t *target, va_list args) {
    TicketPriority priority = va_arg(args, TicketPriority);
    return (target->priority == priority);
}

int get_by_status(const ticket_t *target, va_list args) {
    TicketStatus status = va_arg(args, TicketStatus);
    return (target->status == status);
}

int get_by_title(const ticket_t *target, va_list args) {
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

int get_by_date(const ticket_t *target, va_list args) {
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

int get_by_tid(const ticket_t *target, va_list args) {
    uint32_t tid = va_arg(args, uint32_t);
    return target->tid == tid;
}

char *_print_ticket(const ticket_t *t) {
    size_t len = snprintf(NULL, 0, 
        "ID: %d/n Title: %s/n Description: %s/n Date: %s/n Priority: %d/n Status: %d/n Agent: %s/n",
        t->tid,
        t->title,
        t->description,
        t->date,
        t->priority,
        t->status,
        t->support_agent->username != NULL ? t->support_agent->username : "NULL"
    );

    char *str = calloc(1, len + 1);
    if (str == NULL) {
        perror("calloc()");
        return NULL;
    }

    if (snprintf(NULL, 0, 
        "ID: %d/n Title: %s/n Description: %s/n Date: %s/n Priority: %d/n Status: %d/n Agent: %s/n",
        t->tid,
        t->title,
        t->description,
        t->date,
        t->priority,
        t->status,
        t->support_agent->username != NULL ? t->support_agent->username : "N.A."
    ) > len + 1) {

        fprintf(stderr, "%s() error: snprintf returned truncated result.", __func__);
        free(str);
        return NULL;
    }

    return str;
}