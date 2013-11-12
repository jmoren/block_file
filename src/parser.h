#ifndef _PARSERH_
#define _PARSERH_

#include "record.h"

FILE * open_file_to_parse(char * filename);
void close_entity_file(FILE * file);
int parse_line(char * buffer, struct record_t *record, int max_size);

#endif
