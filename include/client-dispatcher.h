#ifndef CLIENT_DISPATCHER_H
#define CLIENT_DISPATCHER_H

#include "request.h"
#include <stdio.h>

void print_menu();
int compose_request(FILE *fp, request_t **req);

message_t *input_login(FILE *fp);
message_t *input_logout(FILE *fp);
message_t *input_register_user(FILE *fp);
message_t *input_remove_user(FILE *fp);
message_t *input_find_user(FILE *fp);
message_t *input_save_users(FILE *fp);
message_t *input_load_users(FILE *fp);
message_t *input_open_ticket(FILE *fp);
message_t *input_delete_ticket(FILE *fp);
message_t *input_assign_ticket(FILE *fp);
message_t *input_update_ticket_status(FILE *fp);
message_t *input_get_all_tickets(FILE *fp);
message_t *input_ticket_filter_priority(FILE *fp);
message_t *input_ticket_filter_status(FILE *fp);
message_t *input_ticket_filter_support_agent(FILE *fp);
message_t *input_ticket_filter_title(FILE *fp);
message_t *input_ticket_filter_date(FILE *fp);
message_t *input_save_tickets(FILE *fp);
message_t *input_load_tickets(FILE *fp);
message_t *input_quit(FILE *fp);

#endif