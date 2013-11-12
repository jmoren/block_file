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
  int i=0, res;
  FILE * block_file;
  struct file_header * fh;
  struct block_t * block;

  printf("Welcome to Carga Masiva Test\n\n");

  block_file = fopen("block_test.dat", "w");

  fh = (struct file_header*) malloc(sizeof(struct file_header));
  block = (struct block_t *) malloc(sizeof(struct block_t));


  if(!fh || !block)
    return -1;

  initialize_block(block, 1);
  initialize_file_header(fh);

  res = write_header(block_file, fh);
  if(res == 0){
    for(i=0;i<5;i++){
      block->number         = i+1;
      block->total_reg      = i*2+1;
      block->space_occupied = i*5+1;
      fh->total_blocks += 1;
      fh->total_records += 0;
      fh->total_size    += 0;
      write_to_file(block_file, fh, block);
    }
  }

  printf("Listo la carga de bloques\n");
  printf("---------------------------------------\n");
  print_header(fh);

  free(block);
  free(fh);
  fclose(block_file);

  printf("\nCerrando archivos y liberada la memoria\n");
  printf("---------------------------------------\n\n");
  
  fh = (struct file_header*) malloc(sizeof(struct file_header));
  initialize_file_header(fh);
  block_file = fopen("block_test.dat", "r");
  read_header(block_file, fh);
  print_header(fh);

  free(fh);
  fclose(block_file);
  return 0;
}
