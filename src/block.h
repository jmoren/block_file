#ifndef _BLOCKH_
#define _BLOCKH_

#include "record.h"

struct block_t{
  int  number;
  int  total_reg;
  int  space_occupied;
  char data[BLOCK_SIZE-BLOCK_HEADER_SIZE];
};

struct file_header{
  int total_size;
  int total_records;
  int total_blocks;
};

/* File operations */
FILE * open_block_file(char *entidad, int mode);
void close_block_file(FILE * bf);
int write_to_file(FILE * bf, struct file_header * fh, struct block_t * block);
int read_from_file(FILE *block_file, struct block_t * block, struct file_header * fh, int number);
int find_and_write_block(FILE * bf, struct file_header * fh, struct record_t * record, struct block_t * block);

/* Header operations */
int  write_header(FILE * bf, struct file_header * fh);
int  read_header(FILE * bf, struct file_header * fh);
void initialize_file_header(struct file_header * fh);
void print_header(struct file_header * fh);

/* Block operations */
void initialize_block(struct block_t *block, int number);
int write_block(struct block_t * block, struct file_header * fh, struct record_t * record);
int read_block(struct block_t * block, struct record_t * record, int post);

#endif
