#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "ht_table.h"
#include "sht_table.h"

#define RECORDS_NUM 252    // you can change it if you want
#define FILE_NAME "data.db"
#define INDEX_NAME "index.db"

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
  srand(12569874);
  BF_Init(LRU);

  HT_CreateFile(FILE_NAME, BUCKETS_NUM);
  SHT_CreateSecondaryIndex(INDEX_NAME, BUCKETS_NUM, FILE_NAME);
  HT_info* info = HT_OpenFile(FILE_NAME);
  SHT_info* index_info = SHT_OpenSecondaryIndex(INDEX_NAME);
  info = handler;
  index_info = handler2;

  Record record = randomRecord();
  char searchName[15];
  strcpy(searchName, record.name);

  printf("Insert Entries\n");
  int block_id;
  for (int id = 0; id < RECORDS_NUM; ++id) 
  {
    record = randomRecord();
    block_id = HT_InsertEntry(info, record);
    SHT_SecondaryInsertEntry(index_info, record, block_id);
    //printRecord(record);
  }

  strcpy(searchName, "Yannis");
  printf("RUN PrintAllEntries for name %s\n", searchName);
  SHT_SecondaryGetAllEntries(info, index_info, searchName);

  HashStatistics("data.db");
  HashStatistics("index.db");

  SHT_CloseSecondaryIndex(index_info);
  HT_CloseFile(info);
  BF_Close();

  return 0;
}