#include "block.h"

/* ---------------------------------------------------------------------------*/
/* ------------------------ Header operations --------------------------------*/
/* ---------------------------------------------------------------------------*/

void print_header(struct file_header * fh){
  printf(" Cabecera del archivo de datos:\n");
  printf("\tTamaño del archivo: %d bytes\n", fh->total_size);
  printf("\tCantidad de bloques: %d\n", fh->total_blocks);
  printf("\tCantidad de records: %d\n", fh->total_records);
}

int write_header(FILE * bf, struct file_header * fh){
  int fh_size = sizeof(struct file_header);
  int res;
  int total = 1;

  res = fseek(bf, 0, SEEK_SET);
  res = fwrite(fh, fh_size, total, bf);

  if(res == total)
    return 0;
  else
    return -1;
}

int read_header(FILE * bf, struct file_header * fh){
  int res;

  res = fseek(bf, 0, SEEK_SET);
  if(res == 0)
    fread(fh, sizeof(struct file_header), 1, bf);

  return res;
}

void initialize_file_header(struct file_header * fh){
  fh->total_blocks   = 0;
  fh->total_records  = 0;
  fh->total_size     = 0;
}

/* ---------------------------------------------------------------------------*/
/* ---------------------------- Block operations -----------------------------*/
/* ---------------------------------------------------------------------------*/

void initialize_block(struct block_t * block, int number){
  bzero((void*) block, sizeof(struct block_t));
  block->number = number;
  block->space_occupied = sizeof(int)*3;
  block->total_reg = 0;

}

int write_block(struct block_t * block, struct file_header * fh, struct record_t * record){
  int res = 0;
  char * offset;

  /* escribo el record en el bloque*/
  offset = (char*)block + block->space_occupied;
  memcpy(offset, record, sizeof(int) * 2);
  memcpy(offset+sizeof(int)*2, record->content, record->size - sizeof(int)*2);

  /* actualizo los datos de control*/
  block->total_reg += 1;
  block->space_occupied += record->size;

  /* actualizo el file header */
  if(fh->total_blocks < block->number)
    fh->total_blocks += 1;
  fh->total_records += 1;
  fh->total_size += record->size;

  return res;
}

int read_block(struct block_t * block, struct record_t * record, int pos){
  char * offset;

  /* Set offset */
  offset = (char*)block + sizeof(int) * 3 + pos;

  /* Read size */
  memcpy(&(record->size), offset , sizeof(int));

  /* Read code */
  offset = offset + sizeof(int);
  memcpy(&(record->code), offset, sizeof(int));

  /* Read content */
  offset = offset + sizeof(int);
  record->content = (char *) malloc(record->size - (2*sizeof(int)));
  if(!record->content)
    return -1;
  else{
    memcpy(record->content, offset, record->size - (2*sizeof(int)));
    return record->size;
  }
}

/* ---------------------------------------------------------------------------*/
/* ------------------------- Block File operations ---------------------------*/
/* ---------------------------------------------------------------------------*/

FILE * open_block_file(char *entidad, int mode){
  FILE * tmp;
  char *fullpath;

  if (NULL == (fullpath = (char*) malloc(strlen(entidad)+5))) {
    return NULL;
  }else{
    sprintf(fullpath, "%s.dat", entidad);
    switch(mode){
      case 1:
        tmp = fopen(fullpath, "w");
        break;
      case 2:
        tmp = fopen(fullpath, "r");
        break;
    }
    free(fullpath);
  }
  return tmp;
}

int write_to_file(FILE * bf, struct file_header * fh, struct block_t * block){
  int first_block, move_to, status = 0;

  /* Actualizo el header del archivo */
  write_header(bf,fh);

  /* Calculo el desplazamiento */
  first_block = sizeof(struct file_header); /* 12 */
  move_to = (block->number-1) * BLOCK_SIZE + first_block;

  /* Me posiciono en el archivo */
  status = fseek(bf, move_to, SEEK_SET);

  /* Escribo */
  if(status == 0){
    status = fwrite(block, BLOCK_SIZE, 1, bf);
  }
  return status;
}

void close_block_file(FILE * bf){
  fclose(bf);
}

int read_from_file(FILE *block_file, struct block_t * block, struct file_header * fh, int number){
  int first_block, status, move_to;

  if((fh->total_blocks == 0) || (fh->total_blocks < number)){
    status = -5;
  }else{
    first_block = sizeof(struct file_header); /* 12 */
    move_to = ((number-1) * BLOCK_SIZE) + first_block;
    status = fseek(block_file, move_to, SEEK_SET);
    if(status == 0){
      fread(block, BLOCK_SIZE, 1, block_file);
    }
  }
  return status;
}


int find_and_write_block(FILE * bf, struct file_header * fh, struct record_t * record, struct block_t * current_block){

  int record_size = (int) record->size;
  int free_space, res;
  int i = 1;

  free_space = BLOCK_SIZE - current_block->space_occupied;

  if(free_space < record_size){
    res = write_to_file(bf, fh, current_block);
  }
  /* Busco el siguiente bloque disponible o creo uno nuevo*/
  while(free_space < record_size){
    res = read_from_file(bf, current_block, fh, i);
    if(res == -5){
      initialize_block(current_block, fh->total_blocks + 1);
    }
    free_space = BLOCK_SIZE - current_block->space_occupied;
    i++;
  }

  /* Escribo el bloque y actualizo los datos de control */
  res = write_block(current_block, fh, record);

  return res;
}
