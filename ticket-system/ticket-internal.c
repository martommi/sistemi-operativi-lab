#include "ticket-internal.h"
#include "../authentication/user-internal.h"
#include "ticket.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

    if (snprintf(str, len + 1, 
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

int write_string(FILE *fp, const char *str) {
    uint32_t len = strlen(str);
    if (fwrite(&len, sizeof(uint32_t), 1, fp) != 1) return 0;
    if (fwrite(str, sizeof(char), len, fp) != len) return 0;
    return 1;
}

int write_user(FILE *fp, const user_t *u) {
    if (!u) {
        uint8_t null_flag = 1;
        fwrite(&null_flag, sizeof(uint8_t), 1, fp);
        return 1;
    } else {
        uint8_t null_flag = 0;
        fwrite(&null_flag, sizeof(uint8_t), 1, fp);
    }

    if (fwrite(&u->uid, sizeof(uint32_t), 1, fp) != 1) return 0;
    return write_string(fp, u->username);
}

int save_ticket_list(FILE *fp, ticket_t *head) {
    ticket_t *current = head;
    while (current) {
        // Write primitive fields
        if (fwrite(&current->tid, sizeof(uint32_t), 1, fp) != 1) return 0;
        if (!write_string(fp, current->title)) return 0;
        if (!write_string(fp, current->description)) return 0;
        if (!write_string(fp, current->date)) return 0;
        if (fwrite(&current->priority, sizeof(TicketPriority), 1, fp) != 1) return 0;
        if (fwrite(&current->status, sizeof(TicketStatus), 1, fp) != 1) return 0;

        // Write user (support agent)
        if (!write_user(fp, current->support_agent)) return 0;

        // Flag for whether there is a next node
        uint8_t has_next = current->next != NULL;
        if (fwrite(&has_next, sizeof(uint8_t), 1, fp) != 1) return 0;

        current = current->next;
    }
    return 1;
}

int _save_tickets(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("fopen()");
        return -1;
    }

    if (!save_ticket_list(file, head)) {
        fprintf(stderr, "%s(): error saving ticket list.", __func__);
        return -1;
    }

    fclose(file);
    return 1;
}

char *read_string(FILE *fp) {
    uint32_t len;
    if (fread(&len, sizeof(uint32_t), 1, fp) != 1) return NULL;

    char *str = malloc(len + 1);
    if (!str) return NULL;

    if (fread(str, sizeof(char), len, fp) != len) {
        free(str);
        return NULL;
    }

    str[len] = '\0';
    return str;
}

user_t *read_user(FILE *fp) {
    uint8_t null_flag;
    if (fread(&null_flag, sizeof(uint8_t), 1, fp) != 1) return NULL;

    if (null_flag == 1)
        return NULL;

    user_t *u = malloc(sizeof(user_t));
    if (!u) return NULL;

    if (fread(&u->uid, sizeof(uint32_t), 1, fp) != 1) {
        free(u);
        return NULL;
    }

    u->username = read_string(fp);
    if (!u->username) {
        free(u);
        return NULL;
    }

    return u;
}

ticket_t *load_ticket_list(FILE *fp) {
    ticket_t *head = NULL;
    ticket_t *last = NULL;

    while (1) {
        ticket_t *t = malloc(sizeof(ticket_t));
        if (!t) break;

        if (fread(&t->tid, sizeof(uint32_t), 1, fp) != 1) {
            free(t); break;  // fine file?
        }

        t->title = read_string(fp);
        t->description = read_string(fp);
        t->date = read_string(fp);

        if (!t->title || !t->description || !t->date) {
            free(t); break;
        }

        if (fread(&t->priority, sizeof(TicketPriority), 1, fp) != 1) { free(t); break; }
        if (fread(&t->status, sizeof(TicketStatus), 1, fp) != 1) { free(t); break; }

        t->support_agent = read_user(fp);

        uint8_t has_next;
        if (fread(&has_next, sizeof(uint8_t), 1, fp) != 1) { free(t); break; }

        t->next = NULL;

        if (!head) {
            head = t;
        } else {
            last->next = t;
        }
        last = t;

        if (!has_next) break;
    }

    return head;
}

int _load_tickets(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen()");
        return -1;
    }

    ticket_t *head = load_ticket_list(file);

    fclose(file);
    return 1;
}