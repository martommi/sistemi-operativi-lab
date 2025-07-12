#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../../include/utils.h"

void serialize_string(FILE *fp, const char *str) {
    int len = strlen(str);
    fwrite(&len, sizeof(int), 1, fp);
    fwrite(str, sizeof(char), len, fp);
}

char *deserialize_string(FILE *fp) {
    int len;
    char *str;
    fread(&len, sizeof(int), 1, fp);

    str = malloc(sizeof(char) * len);
    if (!str) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    fread(str, sizeof(char), len, fp);
    return str;
}

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

    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);

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
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

int is_valid_path(const char *path, int flags) {
    return access(path, flags) == 0;    /* Checks if path exists and has permissions */
}