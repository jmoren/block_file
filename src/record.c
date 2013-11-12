#include "record.h"

/* ---------------------------------------------------------------------------*/
/* --------------------------- Record operations -----------------------------*/
/* ---------------------------------------------------------------------------*/

void initialize_record(struct record_t * record){
  bzero((struct record_t *) record, sizeof(struct record_t));
  record->code = 0;
  record->size = 0;
}

void print_record(struct record_t * record){
  printf("%d, %s\n", record->code, record->content);
}
