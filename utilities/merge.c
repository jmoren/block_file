int merge_two_files(int file1, int file2, FILE * tmp_file, int (*fp)(struct record_t*, struct record_t*)){
  FILE * tmp_file1;
  FILE * tmp_file2;

  char path1[30], path2[30];
  char * buffer1, * buffer2;
  char *res1, *res2;
  struct record_t *record1, *record2;

  if(file1 == -1){
    sprintf(path1,"tmp/merge%d.txt",file2-2);
  }else{
    sprintf(path1,"tmp/sort%d.txt",file1);
  }

  sprintf(path2,"tmp/sort%d.txt",file2);

  tmp_file1 = fopen(path1, "r");
  tmp_file2 = fopen(path2, "r");

  if(!tmp_file1 || !tmp_file2)
    return BAD_NAME_FILE;

  record1 = (struct record_t*) malloc(sizeof(struct record_t));
  record2 = (struct record_t*) malloc(sizeof(struct record_t));
  buffer1 = (char*) malloc (RECORD_MAX_SIZE);
  buffer2 = (char*) malloc (RECORD_MAX_SIZE);

  if(!buffer1 || !buffer2 || !record1 || !record2)
    return ALLOCATE_FAIL;

  initialize_buffer(buffer1);
  initialize_buffer(buffer2);
  initialize_record(record1);
  initialize_record(record2);

  res1 = fgets(buffer1, RECORD_MAX_SIZE, tmp_file1);
  res2 = fgets(buffer2, RECORD_MAX_SIZE, tmp_file2);

  if(res1){
    parse_line(buffer1, record1, RECORD_MAX_SIZE);
  }
  if(res2){
    parse_line(buffer2, record2, RECORD_MAX_SIZE);
  }
  while(( res1 != NULL) || (res2 != NULL)){
    if(res1 && res2){
      if(fp(record1, record2) >= 0){
        fprintf(tmp_file, "%d,%s\n", record2->code, record2->content);
        free(record2->content);
        initialize_record(record2);
        res2 = fgets(buffer2, RECORD_MAX_SIZE, tmp_file2);
        if(res2){
          parse_line(buffer2, record2, RECORD_MAX_SIZE);
        }
      }else{
        fprintf(tmp_file, "%d,%s\n", record1->code, record1->content);
        res1 = fgets(buffer1, RECORD_MAX_SIZE, tmp_file1);
        free(record1->content);
        initialize_record(record1);
        if(res1){
          parse_line(buffer1, record1, RECORD_MAX_SIZE);
        }
      }
    }

    if(res2 && !res1){
      fprintf(tmp_file, "%d,%s\n", record2->code, record2->content);
      free(record2->content);
      initialize_record(record2);
      res2 = fgets(buffer2, RECORD_MAX_SIZE, tmp_file2);
      if(res2){
        parse_line(buffer2, record2, RECORD_MAX_SIZE);
      }
    }
    if(res1 && !res2){
      fprintf(tmp_file, "%d,%s\n", record1->code, record1->content);
      free(record1->content);
      initialize_record(record1);
      res1 = fgets(buffer1, RECORD_MAX_SIZE, tmp_file1);
      if(res1){
        parse_line(buffer1, record1, RECORD_MAX_SIZE);
      }
    }
  }

  if(record1->content)
    free(record1->content);
  if(record2->content)
    free(record2->content);
  free(buffer1);
  free(buffer2);
  free(record1);
  free(record2);
  fclose(tmp_file1);
  fclose(tmp_file2);

  return 0;
}

int merge_files(int total_files, int (*fp)(struct record_t*, struct record_t*)){
  FILE * tmp_file;
  char * buffer;
  char path[30];
  int res, k=1, i;
  struct record_t * record;

  /* Allocate buffer and record */
  buffer = (char*) malloc (RECORD_MAX_SIZE);
  record = (struct record_t*) malloc(sizeof(struct record_t));

  if(!buffer || !record)
    return ALLOCATE_FAIL;

  initialize_buffer(buffer);
  initialize_record(record);

  if(total_files > 1){
    sprintf(path, "tmp/merge%d.txt", k);
    tmp_file  = fopen(path, "w");
    res = merge_two_files(1, 2, tmp_file, fp);
    fclose(tmp_file);
    for(k=2;k<total_files;k++){
      sprintf(path,"tmp/merge%d.txt", k);
      tmp_file  = fopen(path, "w");
      res = merge_two_files(-1, k+1, tmp_file, fp);
      fclose(tmp_file);
    }
  }

  if(total_files == 1){
    sprintf(path,"tmp/sort%d.txt", total_files);
    tmp_file = fopen(path, "r");
  }
  else{
    sprintf(path,"tmp/merge%d.txt", total_files-1);
    tmp_file = fopen(path, "r");
  }

  /* Abro el ultimo merge */
  i=1;
  while(fgets(buffer, RECORD_MAX_SIZE, tmp_file) != NULL){
    res = parse_line(buffer, record, RECORD_MAX_SIZE);
    printf("%03d) %d, %s\n",i, record->code, record->content);
    free(record->content);
    i++;
  };

  free(buffer);
  free(record);
  fclose(tmp_file);
  return res;
}
