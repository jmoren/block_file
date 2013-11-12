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
  int i=0, res, free_space;
  char content[17] = "Esto es un test  ";
  FILE * block_file;
  struct file_header * fh;
  struct block_t * block;
  struct record_t * record;

  printf("Welcome to Carga Masiva Test\n\n");

  block_file = fopen("block_records_test.dat", "w");

  fh     = (struct file_header*) malloc(sizeof(struct file_header));
  record = (struct record_t*) malloc(sizeof(struct record_t));
  block  = (struct block_t *) malloc(sizeof(struct block_t));


  if(!record || !block || !fh)
    return -1;

  initialize_block(block, 1);
  initialize_record(record);
  initialize_file_header(fh);

  res = write_header(block_file, fh);
  if(res == 0){
    for(i=0;i<50;i++){

      record->content = (char*) malloc(strlen(content)+1);
      memcpy(record->content, content, strlen(content)+1);

      record->content[17] = '\0';
      record->code = i+1;
      record->size = (int)sizeof(record->size) + (int)sizeof(record->code) + (int) strlen(record->content) + 1;

      free_space = MAX_SIZE - block->space_occupied;


      if(free_space < record->size){
        write_to_file(block_file, fh, block);
        printf("(Bloque %d) \tOcupado: %d, Records: %d\n", block->number, block->space_occupied, block->total_reg);
        initialize_block(block, block->number+1);
      }
      write_block(block, fh, record);
      initialize_record(record);
      free(record->content);
    }
  }

  write_to_file(block_file, fh, block);
  printf("(Bloque %d) \tOcupado: %d, Records: %d\n", block->number, block->space_occupied, block->total_reg);
  printf("\nListo la carga de bloques\n");
  printf("---------------------------------------\n");
  print_header(fh);

  free(block);
  free(record);
  free(fh);

  fclose(block_file);
  return 0;
}
