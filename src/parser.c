#include "parser.h"

FILE * open_file_to_parse(char * filename){
  FILE * tmp;
  tmp = fopen(filename, "r");
  return tmp;
}

int parse_line(char * buffer, struct record_t * record, int max_size){
  int i = 0, j = 0;
  char *code, *content;

  if (NULL == (code = (char*) malloc(max_size))) {
    return ALLOCATE_FAIL;
  }
  if (NULL == (content = (char*) malloc(max_size))) {
    free(code);
    return ALLOCATE_FAIL;
  }

  while(buffer[i] != ','){
    code[i] = buffer[i];
    i++;
  }
  code[i] = '\0';
  i++;

  while(buffer[i] != '\n'){
    content[j] = buffer[i];
    j++;
    i++;
  }
  content[j] = '\0';

  if(NULL == (record->content = (char*) malloc(strlen(content)+1))){
    free(code);
    return ALLOCATE_FAIL;
  };

  record->code = atoi(code);
  memcpy(record->content, content, strlen(content)+1);

  record->size = (int)sizeof(int) + (int)sizeof(int) + (int) strlen(record->content) + 1;
  free(code);
  free(content);
  return 0;
}

void close_entity_file(FILE * entity_file){
  if(entity_file)
    fclose(entity_file);
}
