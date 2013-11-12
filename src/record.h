#ifndef _RECORDH_
#define _RECORDH_

#include "definitions.h"

struct record_t{
  int size;
  int code;
  char *content;
};

void initialize_record(struct record_t * record);
void print_record(struct record_t * record);
#endif
