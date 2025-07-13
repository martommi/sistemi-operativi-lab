#include "message.h"
#include <stdio.h>

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