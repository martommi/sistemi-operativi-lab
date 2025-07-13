#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "../../include/utils.h"

const char *get_today() {
    static char buf[11];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
    return buf;
}

int is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int is_valid_date_format(const char *date) {
    if (strlen(date) != 10)
        return 0;

    if (date[4] != '-' || date[7] != '-')
        return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;
        if (!isdigit((unsigned char)date[i]))
            return 0;
    }

    char year_str[5] = {0};
    char month_str[3] = {0};
    char day_str[3] = {0};
    memcpy(year_str, date, 4);
    memcpy(month_str, date + 5, 2);
    memcpy(day_str, date + 8, 2);

    char *endptr;
    int year = (int)strtol(year_str, &endptr, 10);
    int month = (int)strtol(month_str, &endptr, 10);
    int day = (int)strtol(day_str, &endptr, 10);

    if (month < 1 || month > 12)
        return 0;

    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2 && is_leap_year(year))
        days_in_month[1] = 29;

    if (day < 1 || day > days_in_month[month - 1])
        return 0;

    return 1;
}

int is_number(const char *str) {
    if (*str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int is_valid_path(const char *path, int flags) {
    return access(path, flags) == 0;
}

int can_write_file(const char *path) {
    return is_valid_path(path, F_OK | W_OK);
}

int can_read_file(const char *path) {
    return is_valid_path(path, F_OK | R_OK);
}

int read_line(FILE *fp, char *buffer, size_t size) {
    if (!fgets(buffer, size, fp)) return 0;
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

char *prompt_string(FILE *fp, const char *prompt) {
    char buffer[256];
    printf("%s", prompt);
    fflush(stdout);
    if (!read_line(fp, buffer, sizeof(buffer))) return NULL;
    size_t len = strlen(buffer);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, buffer, len + 1);
    return result;
}

char *prompt_validated_input(FILE *fp, const char *prompt, int (*validator)(const char *), const char *error_msg) {
    char *input;
    do {
        input = prompt_string(fp, prompt);
        if (!input) return NULL;

        if (!validator(input)) {
            fprintf(stderr, "%s\n", error_msg);
            free(input);
            input = NULL;
        }
    } while (!input);

    return input;
}