#include "../../include/client-users-dispatcher.h"

#include <stdlib.h>

#include "../../include/utils.h"

message_t *input_login(FILE *fp) {
    char *username = prompt_string(fp, "> username: ");
    char *password = prompt_string(fp, "> password: ");
    if (!username || !password) return NULL;
    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = username;
    content[1] = password;
    return create_message(2, content);
}

message_t *input_logout(FILE *fp) {
    return NULL;    /* No input required */
}

message_t *input_register_user(FILE *fp) {
    char *username = prompt_string(fp, "> choose a username: ");
    char *password = prompt_string(fp, "> choose a password: ");
    if (!username || !password) return NULL;
    char **content = malloc(2 * sizeof(char *));
    if (!content) return NULL;
    content[0] = username;
    content[1] = password;
    return create_message(2, content);
}

message_t *input_remove_user(FILE *fp) {
    char *uid = prompt_validated_input(fp, "> uid of user to delete: ", is_number, "[REMOVE] Invalid UID. Must be a number.\n");
    if (!uid) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = uid;
    return create_message(1, content);
}

message_t *input_find_user(FILE *fp) {
    char *username = prompt_string(fp, "> find username: ");
    if (!username) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = username;
    return create_message(1, content);
}

message_t *input_save_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_write_file, "[SAVE] Invalid path.\n");
    if (!path) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = path;
    return create_message(1, content);
}

message_t *input_load_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_read_file, "[LOAD] Invalid path.\n");
    if (!path) return NULL;
    char **content = malloc(sizeof(char *));
    if (!content) return NULL;
    content[0] = path;
    return create_message(1, content);
}