int compare_code(struct record_t *r1, struct record_t *r2){
  int res;
  if(r1->code < r2->code){
    res = -1;
  }else{
    if(r1->code >= r2->code)
      res = 1;
  }
  return res;
}

int compare_content(struct record_t *r1, struct record_t *r2){
  int res;
  res = strcmp(r1->content, r2->content);
  return res;
}

void print_usage(){
  printf("Modo de uso: \n");
  printf("------------\n");
  printf("./cargador_masivo -i <entidad>\n");
  printf("\t * Carga de datos inicial\n");
  printf("./cargador_masivo -l <entidad>\n");
  printf("\t * Listado de la entidad seleccionada\n");
  printf("./cargador_masivo -k <clave> <entidad>\n");
  printf("\t * Busqueda de la entidad por la clave\n");
  printf("./cargador_masivo -ok <entidad>\n");
  printf("\t * Listado ordenado por clave de la entidad\n");
  printf("./cargador_masivo -on <entidad>\n");
  printf("\t * Listado ordenado por nombre de la entidad\n");
}

void error_handler(int code){
  if (code == BAD_NUM_OF_PARAMS){
    printf("!!! Error (%d)\t", code);
    printf("Cantidad de parametros no permitida para esta operacion\n\n");
  }
  if (code == UNKNOWN_PARAM){
    printf("!!! Error (%d)\t", code);
    printf("Opcion de procesamiento inexistente\n\n");
  }
  if (code == BAD_NAME_FILE){
    printf("!!! Error (%d)\t", code);
    printf("No se pudo abrir el archivo correspondiente\n\n");
  }
  if (code == ERROR_READ_BLOCK){
    printf("!!! Error (%d)\t", code);
    printf("No se puede leer registros de un bloque\n\n");
  }

  if(code != 0)
    print_usage();
}

int check_params(int params, int allowed){
  int res = 0;

  if(allowed == 0){
    if((params > 4) || (params < 3))
      res = BAD_NUM_OF_PARAMS;
  }else{
    if(params != allowed)
      res = BAD_NUM_OF_PARAMS;
  }

  return res;
}

void initialize_buffer(char * buffer){
  bzero((void *) buffer, (int) RECORD_MAX_SIZE);
}
