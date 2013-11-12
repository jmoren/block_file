#include "../src/record.h"

void heapify(struct record_t array[], int n, int (*fp)(struct record_t*, struct record_t*)){
  int i,j,k;
  struct record_t record;

  for(k=1 ; k<n ; k++){
    memcpy(&record,&array[k], sizeof(struct record_t));
    i = k;
    j = (i-1)/2;
    while( (i>0) && (fp(&record,&array[j]) < 0 )){
      memcpy(&array[i],&array[j], sizeof(struct record_t));
      i = j;
      j = (i-1)/2;
    }
    memcpy(&array[i],&record, sizeof(struct record_t));
  }
}

void update(struct record_t array[], int n, int (*fp)(struct record_t*, struct record_t*)){
  int i,j;
  struct record_t record;
  j = 0;
  memcpy(&record,&array[j], sizeof(struct record_t));
  i = 2*j+1;
  while(i<=n-1){
    if(i+1 <= n-1)
      if( fp(&array[i],&array[i+1]) > 0)
        i++;
    if(fp(&record, &array[i]) > 0){
      memcpy(&array[j],&array[i], sizeof(struct record_t));
      j = i;
      i = 2*j+1;
    }else
      break;
  }
  memcpy(&array[j], &record, sizeof(struct record_t));
}
void heapsort(struct record_t array[], int n, int (*fp)(struct record_t*, struct record_t*)){
  int i;
  struct record_t record;
  heapify(array,n,fp);
  for(i=n-1 ; i>0 ; i--){
    /* swap elems */
    memcpy(&record,&array[0], sizeof(struct record_t));
    memcpy(&array[0],&array[i], sizeof(struct record_t));
    memcpy(&array[i],&record, sizeof(struct record_t));
    update(array,i,fp);
  }
}
