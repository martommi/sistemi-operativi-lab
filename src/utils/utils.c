#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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