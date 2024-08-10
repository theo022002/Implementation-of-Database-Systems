#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "ht_file.h"

#define RECORDS_NUM 500 // you can change it if you want

#define FILE_NAME "data.db"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int main(void) 
{
  BF_Init(LRU);
  HT_CreateFile(FILE_NAME, BUCKETS_NUM);
  HT_info* info = HT_OpenFile(FILE_NAME);
  info = handler;

  Record record;
  srand(12569874);
  int r;
  printf("Insert Entries\n");
  int a;
  for (int id = 0; id < RECORDS_NUM; ++id) 
  {
    record = randomRecord();
    a = HT_InsertEntry(info, record);
    //printRecord(record);
  }

  int num = 0;
  BF_GetBlockCounter(handler->fileDesc, &num);
  printf("The blocks in the file are: %d\n", num);

  printf("RUN PrintAllEntries\n");
  int id = rand() % RECORDS_NUM;
  printf("\nSearching for: %d\n",id);
  HT_GetAllEntries(info, id);

  HT_CloseFile(info);
  BF_Close();
}