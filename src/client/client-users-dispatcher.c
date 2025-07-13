#include "../../include/client-users-dispatcher.h"
#include "../../include/utils.h"

message_t *input_login(FILE *fp) {
    char *username = prompt_string(fp, "> username: ");
    char *password = prompt_string(fp, "> password: ");
    if (!username || !password) return NULL;
    char *content[] = { username, password };
    return create_message(2, content);
}

message_t *input_logout(FILE *fp) {
    return NULL;    /* No input required */
}

message_t *input_register_user(FILE *fp) {
    return input_login(fp);   //TODO non mi piace che si chiamano
}

message_t *input_remove_user(FILE *fp) {
    char *uid = prompt_validated_input(fp, "> uid of user to delete: ", is_number, "[REMOVE] Invalid UID. Must be a number.");
    if (!uid) return NULL;
    char *content[] = { uid };
    return create_message(1, content);
}

message_t *input_find_user(FILE *fp) {
    char *username = prompt_string(fp, "> find username: ");
    if (!username) return NULL;
    char *content[] = { username };
    return create_message(1, content);
}

message_t *input_save_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_write_file, "[SAVE] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}

message_t *input_load_users(FILE *fp) {
    char *path = prompt_validated_input(fp, "> file path: ", can_read_file, "[LOAD] Invalid path.");
    if (!path) return NULL;
    char *content[] = { path };
    return create_message(1, content);
}