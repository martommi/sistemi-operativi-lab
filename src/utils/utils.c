#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "../../include/utils.h"

void serialize_string(FILE *fp, const char *str) {
    if (!str) {
        fprintf(stderr, "serialize_string: null pointer\n");
        exit(EXIT_FAILURE);
    }

    uint32_t len = strlen(str);
    uint32_t net_len = htonl(len);

    if (fwrite(&net_len, sizeof(uint32_t), 1, fp) != 1 ||
        fwrite(str, sizeof(char), len, fp) != len) {
        perror("fwrite()");
        exit(EXIT_FAILURE);
    }
}

char *deserialize_string(FILE *fp) {
    uint32_t net_len, len;

    if (fread(&net_len, sizeof(uint32_t), 1, fp) != 1) {
        perror("fread(len)");
        exit(EXIT_FAILURE);
    }

    len = ntohl(net_len);

    char *str = malloc(len + 1);
    if (!str) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    if (fread(str, sizeof(char), len, fp) != len) {
        perror("fread(str)");
        free(str);
        exit(EXIT_FAILURE);
    }

    str[len] = '\0';
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
    return read_line(fp, buffer, sizeof(buffer)) ? strdup(buffer) : NULL;
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

ssize_t write_all(int fd, const void *buf, size_t count) {
    size_t written = 0;
    while (written < count) {
        ssize_t res = write(fd, (const char *)buf + written, count - written);
        if (res <= 0) return -1;
        written += res;
    }
    return written;
}

ssize_t read_all(int fd, void *buf, size_t count) {
    size_t read_bytes = 0;
    while (read_bytes < count) {
        ssize_t res = read(fd, (char *)buf + read_bytes, count - read_bytes);
        if (res <= 0) return -1;
        read_bytes += res;
    }
    return read_bytes;
}