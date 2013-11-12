#include "src/record.h"
#include "src/parser.h"
#include "src/block.h"

#include "utilities/helpers.c"
#include "utilities/merge.c"
#include "utilities/heapsort.c"

/* ===========================================================================*/
/* ===========================================================================*/
/* ========================== Load data (option -i) ==========================*/
/* ===========================================================================*/
/* ===========================================================================*/

int load_data(char *filename, char *entidad){
  FILE * entity_file;
  FILE * block_file;
  struct record_t *record;
  struct file_header * file_header;
  struct block_t * current_block;
  char * buffer;
  int res;

  /* Entity file */
  entity_file = open_file_to_parse(filename);
  if (!entity_file){
    return BAD_NAME_FILE;
  }
  /* Block and header file */
  block_file = open_block_file(entidad, 1);
  if(!block_file){
    return BAD_NAME_FILE;
  }else{
    file_header = (struct file_header*) malloc(sizeof(struct file_header));
    if(!file_header)
      return ALLOCATE_FAIL;
    else{
      initialize_file_header(file_header);
      write_header(block_file, file_header);
    }
  }

  /* Allocate buffer and record */
  buffer = (char*) malloc (RECORD_MAX_SIZE);
  record = (struct record_t*) malloc(sizeof(struct record_t));
  current_block = (struct block_t *) malloc(sizeof(struct block_t));

  if(!buffer || !record || !current_block)
    return ALLOCATE_FAIL;

  initialize_buffer(buffer);
  initialize_block(current_block, 1);
  initialize_record(record);

  /* Read text file */
  while(fgets(buffer, RECORD_MAX_SIZE, entity_file) != NULL){
    res = parse_line(buffer, record, RECORD_MAX_SIZE);
    if(res == RES_OK){
      res = find_and_write_block(block_file, file_header, record, current_block);
      initialize_record(record);
      free(record->content);
    };
  };
  write_to_file(block_file, file_header, current_block);
  print_header(file_header);

  free(record);
  free(buffer);
  free(file_header);
  free(current_block);
  close_entity_file(entity_file);
  close_block_file(block_file);
  return res;
}

/* ===========================================================================*/
/* ===========================================================================*/
/* ========================== List data (option -l) ==========================*/
/* ===========================================================================*/
/* ===========================================================================*/

int list_data(char *filename){
  FILE * block_file;
  struct record_t *record;
  struct block_t * current_block;
  struct file_header * file_header;
  int res, i, j, recs, k;
  int current_pos = 0;

  block_file = open_block_file(filename, 2);
  if(!block_file)
    return BAD_NAME_FILE;

  /* Allocate buffer and record */
  record        = (struct record_t*) malloc(sizeof(struct record_t));
  current_block = (struct block_t *) malloc(sizeof(struct block_t));
  file_header   = (struct file_header*) malloc(sizeof(struct file_header));

  if(!record || !current_block || !file_header)
    return ALLOCATE_FAIL;

  initialize_record(record);
  initialize_block(current_block,1);
  read_header(block_file, file_header);

  printf("Listado de %s\n\n", filename);
  k=0;
  for(i=1;i<file_header->total_blocks+1; i++){
    current_pos = 0;
    res  = read_from_file(block_file, current_block, file_header, i);
    if(res == RES_OK){
      recs = current_block->total_reg;
      for(j=0; j < recs; j++){
        initialize_record(record);
        current_pos += read_block(current_block, record, current_pos);
        printf("%03d) %d, %s\n",k+1, record->code, record->content);
        k++;
        free(record->content);
      }
    }
  }
  printf("\n");
  free(record);
  free(file_header);
  free(current_block);
  fclose(block_file);
  return res;
}

/* ===========================================================================*/
/* ===========================================================================*/
/* ===================== Search by code (option -k) ==========================*/
/* ===========================================================================*/
/* ===========================================================================*/
void print_success_message(struct record_t * record){
  printf("\n Registro encontrado!! ");
  printf(" Codigo: %d - Nombre: %s\n\n", record->code, record->content);
}

void print_error_message(int code){
  printf("\n No se econtro ningun registro!! ");
  printf(" Codigo buscado => %d\n\n", code);
}

int search_by_code(char* entidad, int code){
  FILE * block_file;
  struct record_t *record;
  struct block_t * current_block;
  struct file_header * file_header;
  int res, i = 1, j = 1, recs = 0;
  int current_pos = 0, encontrado = 1;

  block_file = open_block_file(entidad, 2);
  if(!block_file){
    return BAD_NAME_FILE;
  }

  /* Allocate buffer and record */
  record        = (struct record_t*) malloc(sizeof(struct record_t));
  current_block = (struct block_t *) malloc(sizeof(struct block_t));
  file_header   = (struct file_header*) malloc(sizeof(struct file_header));

  if(!record || !current_block || !file_header)
    return ALLOCATE_FAIL;

  initialize_record(record);
  initialize_block(current_block,1);
  read_header(block_file, file_header);

  while((i < file_header->total_blocks+1) && (encontrado == 1)){
    current_pos = 0;
    j = 1;
    res  = read_from_file(block_file, current_block, file_header, i);
    if(res == RES_OK){
      recs = current_block->total_reg;
      while((j < recs+1) && (encontrado == 1)){
        initialize_record(record);
        current_pos += read_block(current_block, record, current_pos);
        if(code == record->code){
          print_success_message(record);
          encontrado = 0;
        }
        free(record->content);
        j++;
      }
    }
    i++;
  }
  if(encontrado == 1){
    print_error_message(code);
  }
  free(record);
  free(file_header);
  free(current_block);
  fclose(block_file);
  return res;
}

/* ===========================================================================*/
/* ===========================================================================*/
/* ============= Order data by code or content (option -ok|-on) ==============*/
/* ===========================================================================*/
/* ===========================================================================*/

void print_record_to_file(struct record_t buffer[], FILE * sort_file){
  fprintf(sort_file,"%d, %s\n", buffer[0].code, buffer[0].content);
  if(buffer[0].content){
    free(buffer[0].content);
  }
}

void print_buffer_to_file(struct record_t buffer[], int size, FILE * sort_file, int (*fp)(struct record_t*, struct record_t*)){
  int i;
  heapsort(buffer,size, fp);
  for(i=size-1;i>=0;i--){
    fprintf(sort_file,"%d, %s\n", buffer[i].code, buffer[i].content);
    if(buffer[i].content){
      free(buffer[i].content);
    }
  }
}

int order(char * entidad, int (*fp)(struct record_t*, struct record_t*)){
  FILE * block_file;
  FILE * tmp_file;
  struct record_t *record;
  struct block_t * current_block;
  struct file_header * file_header;

  int res, j = 0;
  int current_pos = 0;
  int current_pos_freeze = 0;
  int registros_leidos = 0, bloques_leidos = 0;
  int todo_leido = 1, totales_leidos = 0;
  int registro_en_memoria = 1, registro_procesado = 0;
  int tmp_files = 1;
  char path[30];

  struct record_t buffer[TMP_BUFFER];
  struct record_t freeze_buffer[TMP_BUFFER];

  block_file = open_block_file(entidad, 2);

  if(!block_file){
    return BAD_NAME_FILE;
  }

  /* Allocate buffer and record */
  current_block = (struct block_t *) malloc(sizeof(struct block_t));
  file_header   = (struct file_header*) malloc(sizeof(struct file_header));
  record  = (struct record_t*) malloc(sizeof(struct record_t));

  if(!current_block || !file_header || !record)
    return ALLOCATE_FAIL;

  initialize_block(current_block,1);
  read_header(block_file, file_header);
  initialize_record(record);

  res  = read_from_file(block_file, current_block, file_header, 1);
  bloques_leidos++;

  if(res == RES_OK){

    /* Inicializo el buffer con registros */
    while(j<TMP_BUFFER && (todo_leido == 1)){
      if(registros_leidos < current_block->total_reg){
        current_pos += read_block(current_block, record, current_pos);
        memcpy(&buffer[j],record,sizeof(struct record_t));
        registros_leidos++;
        totales_leidos++;
        j++;
      }else{
        if((bloques_leidos < file_header->total_blocks+1) && (file_header->total_records > totales_leidos)){
          res = read_from_file(block_file, current_block, file_header, current_block->number+1);
          bloques_leidos++;
          registros_leidos = 0;
          current_pos = 0;
        }else{
          todo_leido = 0;
        }
      }
    }

    /* Creo el primer sort file */
    sprintf(path,"tmp/sort%d.txt",tmp_files);
    tmp_file = fopen(path, "w");

    if(file_header->total_records == totales_leidos)
      heapify(buffer,totales_leidos, fp);
    else
      heapify(buffer,TMP_BUFFER, fp);

    while((todo_leido == 1) && tmp_file){
      if(registros_leidos < current_block->total_reg){
        current_pos += read_block(current_block, record, current_pos);
        registros_leidos++;
        registro_en_memoria = 0;
        registro_procesado  = 1;
      }else{
        if(bloques_leidos < file_header->total_blocks){
          res  = read_from_file(block_file, current_block, file_header, current_block->number+1);
          bloques_leidos++;
          current_pos = 0;
          registros_leidos    = 0;
          registro_procesado  = 1;
          registro_en_memoria = 1;
        }else{
          todo_leido = 0;
        }
      }

      while((registro_procesado == 1) && (registro_en_memoria == 0)){
        if(fp(record,&buffer[0]) < 0){
          if(current_pos_freeze<TMP_BUFFER){
            memcpy(&freeze_buffer[current_pos_freeze],record,sizeof(struct record_t));
            current_pos_freeze++;
            registro_procesado  = 0;
            registro_en_memoria = 1;
          }
          else {
            /* vacio el buffer en el archivo */
            print_buffer_to_file(buffer, TMP_BUFFER, tmp_file, fp);
            /* cierro archivo y abro el siguiente */
            tmp_files++;
            fclose(tmp_file);
            sprintf(path,"tmp/sort%d.txt",tmp_files);
            tmp_file = fopen(path, "w");
            /* copio el freeze_buffer al buffer y creo el heap*/
            heapify(freeze_buffer,TMP_BUFFER, fp);
            memcpy(buffer,freeze_buffer,sizeof(struct record_t) * TMP_BUFFER);
            current_pos_freeze = 0;
            registro_procesado = 1;
          }
        }
        else{
          /* Guardo en el archivo, libero memoria */
          print_record_to_file(buffer, tmp_file);
          /* Guardo en el buffer el record leido */
          memcpy(&buffer[0],&buffer[TMP_BUFFER-1],sizeof(struct record_t));
          memcpy(&buffer[TMP_BUFFER-1],record,sizeof(struct record_t));
          registro_procesado  = 0;
          registro_en_memoria = 1;
          update(buffer, TMP_BUFFER, fp);
        }
      }
    }
    /* Guardo en el archivo*/
    if((file_header->total_records == totales_leidos) && (current_block->number == 1))
      print_buffer_to_file(buffer, totales_leidos, tmp_file, fp);
    else
      print_buffer_to_file(buffer, TMP_BUFFER, tmp_file, fp);

    fclose(tmp_file);

    /* Guardo lo que tenia en el freeze_buffer en otro archivo */
    if(current_pos_freeze > 1){
      tmp_files++;
      sprintf(path,"tmp/sort%d.txt",tmp_files);
      tmp_file = fopen(path, "w");
      print_buffer_to_file(freeze_buffer, current_pos_freeze, tmp_file, fp);
      fclose(tmp_file);
    }
  }

  free(record);
  free(current_block);
  free(file_header);
  fclose(block_file);

  /* Merge de los archivos generados */
  res = merge_files(tmp_files, fp);
  printf("\n");

  return res;
}

/* ===========================================================================*/
/* ===========================================================================*/
/* ================== Order data by name (option -on) ========================*/
/* ===========================================================================*/
/* ===========================================================================*/

int order_by_code(char * entidad){
  int res;
  int (*fp)(struct record_t *, struct record_t *);
  fp = compare_code;

  printf("Listado de entidad '%s' ordenado por clave\n\n", entidad);
  res = order(entidad, fp);
  return res;
}

int order_by_name(char * entidad){
  int res;
  int (*fp)(struct record_t *, struct record_t *);
  fp = compare_content;

  printf("Listado de entidad '%s' ordenado por nombre\n\n", entidad);
  res = order(entidad, fp);
  return res;
}

int main(int argc, char **argv){
  int res = 0, code;
  char *file_name;
  char *operation;
  char *entidad;

  printf("\n\033[1;37m Tp0 - Cargador Masivo\n");
  printf(" ---------------------\n\033[0m");

  res = check_params(argc, 0);
  if(res == RES_OK){
    operation = argv[1];
    res = UNKNOWN_PARAM;

    if (strncmp(operation, "-i", 2) == 0 ){
      res = check_params(argc, 4);
      if(res == RES_OK){
        entidad   = argv[2];
        file_name = argv[3];
        res = load_data(file_name, entidad);
      }
    }
    if (strncmp(operation, "-l", 2) == 0 ){
      res = check_params(argc, 3);
      if(res == RES_OK){
        entidad   = argv[2];
        res = list_data(entidad);
      }
    }
    if (strncmp(operation, "-k", 2) == 0 ){
      res = check_params(argc, 4);
      if(res == RES_OK){
        code = atoi(argv[2]);
        entidad = argv[3];
        res = search_by_code(entidad, code);
      }
    }
    if (strncmp(operation, "-ok", 3) == 0 ){
      res = check_params(argc, 3);
      if(res == RES_OK){
        entidad = argv[2];
        res = order_by_code(entidad);
      }
    }
    if (strncmp(operation, "-on", 3) == 0 ){
      res = check_params(argc, 3);
      if(res == RES_OK){
        entidad = argv[2];
        res = order_by_name(entidad);
      }
    }
  }
  error_handler(res);
  return res;
}
