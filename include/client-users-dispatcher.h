#include "message.h"
#include <stdio.h>

message_t *input_login(FILE *fp);
message_t *input_logout(FILE *fp);
message_t *input_register_user(FILE *fp);
message_t *input_remove_user(FILE *fp);
message_t *input_find_user(FILE *fp);
message_t *input_save_users(FILE *fp);
message_t *input_load_users(FILE *fp);