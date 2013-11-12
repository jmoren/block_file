#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../src/definitions.h"
#include "../src/parser.h"
#include "../src/record.h"
#include "../src/block.h"


int MAX_SIZE = 200;

int main(int argc, char **argv){
  int i=0, current_pos;
  char content[17] = "Esto es un test  ";
  struct block_t * block;
  struct record_t * record;
  struct file_header * file_header;

  printf("Welcome to Carga Masiva Test\n\n");


  file_header = (struct file_header*) malloc(sizeof(struct file_header));
  record = (struct record_t*) malloc(sizeof(struct record_t));
  block  = (struct block_t *) malloc(sizeof(struct block_t));


  if(!record || !block || !file_header)
    return -1;

  initialize_block(block, 1);
  initialize_record(record);
  initialize_file_header(file_header);

  for(i=0;i<5;i++){
    record->content = (char*) malloc(strlen(content)+1);
    memcpy(record->content, content, strlen(content)+1);

    record->content[17] = '\0';
    record->code = i+1;
    record->size = (int)sizeof(record->size) + (int)sizeof(record->code) + (int) strlen(record->content) + 1;
    write_block(block, file_header, record);
    free(record->content);
  }

  printf(" Cargados en el bloque:\n Numero: %d, Registros: %d, Espacio ocupado: %d\n", block->number, block->total_reg, block->space_occupied);

  initialize_record(record);
  current_pos = 0;
  for(i=0; i < 5; i++){
    initialize_record(record);
    current_pos += read_block(block, record, current_pos);
    printf(" %d, %s\n",record->code, record->content);
    free(record->content);
  }

  free(block);
  free(record);
  return 0;
}
