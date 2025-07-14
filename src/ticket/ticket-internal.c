#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "ticket-internal.h"
#include "../auth/user-internal.h"
#include "../../include/string-utils.h"
#include "../../include/file-utils.h"
#include "../../include/ticket.h"

static ticket_t *head = NULL;
static uint32_t tid = 1;

ticket_t *_create_ticket(char *title, char *desc, const char *date, TicketPriority priority, TicketStatus status, user_t *support_agent) {
    if (!title || !desc || !date || !priority || !status) {
        fprintf(stderr, "%s(): Invalid args.\n", __func__);
        return NULL;
    }

    ticket_t *ticket;
    if ((ticket = (ticket_t *)malloc(sizeof(ticket_t))) == NULL) {
        perror("malloc()");
        return NULL;
    }

    memset(ticket, 0, sizeof(ticket_t));

    ticket->tid = tid++;
    ticket->title = strdup(title);
    ticket->description = strdup(desc);
    ticket->date = strdup(date);
    ticket->priority = priority;
    ticket->status = status;
    ticket->support_agent = support_agent;    /* Nullable */
    ticket->next = NULL;

    return ticket;
}

void _free_ticket(ticket_t *target) {
    if (target == NULL) return;

    if (target->title) free(target->title);
    if (target->description) free(target->description);
    if (target->date) free(target->date);
    free(target);
}

int _get_tickets(ticket_t ***destination, ticket_filter filter, ...) {
    ticket_t *current = head;
    int count = 0;
    ticket_t **result = NULL;

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
            ticket_t **tmp = realloc(result, (count + 1) * sizeof(ticket_t *));
            if (!tmp) {
                perror("realloc()");
                free(result);
                *destination = NULL;
                va_end(args);
                return -1;
            }
            result = tmp;
            result[count++] = current;
        }

        current = current->next;
    }

    va_end(args);
    *destination = result;
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
    char *ticket_date = target->date;
    if (!ticket_date) return 0;
    strip_newline(ticket_date);

    DateMatchMode mode = va_arg(args, DateMatchMode);

    switch (mode) {
        case DATE_MATCH_EXACT: {
            char *date = va_arg(args, char *);
            strip_newline(date);
            return strcmp(ticket_date, date) == 0;
        }

        case DATE_MATCH_BEFORE: {
            char *date = va_arg(args, char *);
            strip_newline(date);
            return strcmp(ticket_date, date) < 0;
        }

        case DATE_MATCH_AFTER: {
            char *date = va_arg(args, char *);
            strip_newline(date);
            return strcmp(ticket_date, date) > 0;
        }

        case DATE_MATCH_RANGE: {
            char *start = va_arg(args, char *);
            char *end = va_arg(args, char *);
            strip_newline(start);
            strip_newline(end);
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

int get_by_support_agent(const ticket_t *target, va_list args) {
    char *username = va_arg(args, char *);
    return (target->support_agent) ? (strcmp(target->support_agent->username, username) == 0) : 0;
}

int _add_ticket(ticket_t *ticket) {
    ticket_t *curr = head;

    while (curr) {
        if (curr->tid == ticket->tid) {
            fprintf(stderr, "%s(): ticket with TID %u already registered.\n", __func__, ticket->tid);
            return -1;
        }
        
        curr = curr->next;
    }

    ticket->next = head;
    head = ticket;
    return 1;
}

int _remove_ticket(uint32_t tid) {
    ticket_t *curr = head;
    ticket_t *prev = NULL;

    while (curr) {
        if (curr->tid == tid) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }

            _free_ticket(curr);
            return 1;
        }

        prev = curr;
        curr = curr->next;
    }

    fprintf(stderr, "%s(): trying to remove non-existing ticket.\n", __func__);
    return -1;
}

int _count_tickets() {
    int count = 0;
    for (ticket_t *cur = head; cur; cur = cur->next)
        count++;

    return count;
}

int _set_support_agent(ticket_t *ticket, user_t *support_agent) {
    ticket->support_agent = support_agent;
    return 1;
}

int _set_status(ticket_t *ticket, TicketStatus new_status) {
    ticket->status = new_status;
    return 1;
}

char *_print_ticket(const ticket_t *t) {
    size_t len = snprintf(NULL, 0,
        "ID: %d\n Title: %s\n Description: %s\n Date: %s\n Priority: %s\n Status: %s\n Agent: %s\n",
        t->tid,
        t->title,
        t->description,
        t->date,
        _print_priority(t->priority),
        _print_status(t->status),
        (t->support_agent) ? t->support_agent->username : "N.A."
    );

    char *str = calloc(1, len + 1);
    if (str == NULL) {
        perror("calloc()");
        return NULL;
    }

    if (snprintf(str, len + 1, 
        "ID: %d\n Title: %s\n Description: %s\n Date: %s\n Priority: %s\n Status: %s\n Agent: %s\n",
        t->tid,
        t->title,
        t->description,
        t->date,
        _print_priority(t->priority),
        _print_status(t->status),
        (t->support_agent) ? t->support_agent->username : "N.A."
    ) > len) {

        fprintf(stderr, "%s() error: snprintf returned truncated result.\n", __func__);
        free(str);
        return NULL;
    }

    return str;
}

int _serialize_ticket(int fd, const ticket_t *t) {
    if (t == NULL) {
        fprintf(stderr, "%s(): null ticket.\n", __func__);
        return -1;
    }

    uint32_t tid_n = htonl(t->tid);
    if (write_all(fd, &tid_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write failed.\n", __func__);
        return -1;
    }

    if (!serialize_string(fd, t->title) ||
        !serialize_string(fd, t->description) ||
        !serialize_string(fd, t->date)) {
        
        fprintf(stderr, "%s(): write failed.\n", __func__);
        return -1;
    }

    uint32_t pr = htonl(t->priority);
    uint32_t st = htonl(t->status);
    if (write_all(fd, &pr, sizeof(uint32_t)) != sizeof(uint32_t) ||
        write_all(fd, &st, sizeof(uint32_t)) != sizeof(uint32_t)) {
        
        fprintf(stderr, "%s(): write failed.\n", __func__);
        return -1;
    }

    uint8_t has_agent = (t->support_agent != NULL) ? 1 : 0;
    if (write_all(fd, &has_agent, sizeof(uint8_t)) != sizeof(uint8_t)) {
        fprintf(stderr, "%s(): write failed (agent flag).\n", __func__);
        return -1;
    }

    if (has_agent) {
        if (_serialize_user(fd, t->support_agent) != 1) {
            fprintf(stderr, "%s(): user serialization failed.\n", __func__);
            return -1;
        }
    }

    return 1;
}

ticket_t *_deserialize_ticket(int fd) {
    ticket_t *ticket = malloc(sizeof(ticket_t));
    if (!ticket) {
        perror("malloc()");
        return NULL;
    }

    uint32_t tid_n;
    if (read_all(fd, &tid_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.\n", __func__);
        free(ticket);
        return NULL;
    }

    ticket->tid = ntohl(tid_n);

    ticket->title = deserialize_string(fd);
    ticket->description = deserialize_string(fd);
    ticket->date = deserialize_string(fd);

    if (!ticket->title || !ticket->description || !ticket->date) {
        free(ticket->title);
        free(ticket->description);
        free(ticket->date);
        free(ticket);
        return NULL;
    }

    uint32_t pr_n, st_n;
    if (read_all(fd, &pr_n, sizeof(uint32_t)) != sizeof(uint32_t) ||
        read_all(fd, &st_n, sizeof(uint32_t)) != sizeof(uint32_t)) {

        fprintf(stderr, "%s(): read failed.\n", __func__);
        free(ticket->title);
        free(ticket->description);
        free(ticket->date);
        free(ticket);
        return NULL;
    }
    
    ticket->priority = (TicketPriority)ntohl(pr_n);
    ticket->status = (TicketStatus)ntohl(st_n);

    uint8_t has_agent;
    if (read_all(fd, &has_agent, sizeof(uint8_t)) != sizeof(uint8_t)) {
        fprintf(stderr, "%s(): read failed (agent flag).\n", __func__);
        free(ticket->title);
        free(ticket->description);
        free(ticket->date);
        free(ticket);
        return NULL;
    }

    if (has_agent) {
        user_t *user = _deserialize_user(fd);

        if (user == NULL || !_add_user(user)) {
            _free_user(user);
            ticket->support_agent = NULL;
        } else {
            ticket->support_agent = user;
        }
    } else {
        ticket->support_agent = NULL;
    }

    ticket->next = NULL;
    return ticket;
}

int _save_tickets(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    uint32_t count_n = htonl(_count_tickets());
    if (write_all(fd, &count_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): write failed.\n", __func__);
        close(fd);
        return -1;
    }

    ticket_t *current = head;
    while (current) {
        if (_serialize_ticket(fd, current) < 0) {
            fprintf(stderr, "%s(): failed to serialize.", __func__);
            close(fd);
            return -1;
        }

        current = current->next;
    }

    close(fd);
    return 1;
}

int _load_tickets(const char *filename) {
    int fd = open(filename, O_RDONLY, 0644);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    uint32_t count_n;
    if (read_all(fd, &count_n, sizeof(uint32_t)) != sizeof(uint32_t)) {
        fprintf(stderr, "%s(): read failed.\n", __func__);
        close(fd);
        return -1;
    }

    uint32_t count = ntohl(count_n);

    while (count > 0) {
        ticket_t *ticket = _deserialize_ticket(fd);
        if (!ticket) {
            close(fd);
            return -1;
        }

        _add_ticket(ticket);
        count--;
    }
    
    close(fd);
    return 1;
}

char *_print_priority(TicketPriority p) {
    switch (p) {
        case TICKET_PRIORITY_LOW:
            return "Low";
        case TICKET_PRIORITY_STANDARD:
            return "Standard";
        case TICKET_PRIORITY_HIGH:
            return "High";
        default:
            return "N.A.";
    }
}

char *_print_status(TicketStatus s) {
    switch (s) {
    case TICKET_STATUS_OPEN:
        return "Open";
    case TICKET_STATUS_ONGOING:
        return "Ongoing";
    case TICKET_STATUS_CLOSED:
        return "Closed";
    default:
        return "N.A.";
    }
}