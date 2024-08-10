#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sht_table.h"
#include "ht_table.h"
#include "record.h"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int hash_function2(char* name, int base)
{
  int len = strlen(name);
  return (len % base + 1);
}

int SHT_CreateSecondaryIndex(char *sfileName, int buckets, char* fileName)
{
  handler2 = malloc(sizeof(SHT_info));
  handler2->block0 = malloc(sizeof(SHT_block));
  handler2->block0->b = malloc(sizeof(SHT_block_info));
  handler2->numBuckets = buckets;
  for (int i = 0; i <= handler2->numBuckets; i++)
  {
    handler2->BLOCKS[i][0] = malloc(sizeof(SHT_block));
  }
  for (int i = 0; i <= handler2->numBuckets; i++)
  {
    handler2->BLOCKS[i][0]->b = malloc(sizeof(SHT_block_info));
  }
  handler2->fileDesc = 0;
  handler2->count_b = 0;
  for (int i = 0; i <= handler2->numBuckets; i++)
  {
    handler2->count_overflow[i] = 0;
  }
  
  CALL_OR_DIE(BF_CreateFile(sfileName));
  return 0;
}

SHT_info* SHT_OpenSecondaryIndex(char *indexName)
{
  handler2->fileDesc++;
  BF_ErrorCode code = BF_OpenFile(indexName, &(handler2->fileDesc));

  BF_Block_Init(&handler2->block0->block);                                        
  CALL_OR_DIE(BF_AllocateBlock(handler2->fileDesc, handler2->block0->block));      
  handler2->count_b++;

  if (code != BF_OK)
    return NULL;
}


int SHT_CloseSecondaryIndex(SHT_info* SHT_info)
{
  BF_ErrorCode code = BF_CloseFile(handler2->fileDesc);

  if (code != BF_OK)
    return -1;
  else 
  {
    for (int i = 0; i <= handler2->numBuckets; i++)
    {
      free(handler2->BLOCKS[i][0]->b);
    }
    for (int i = 0; i <= handler2->numBuckets; i++)
    {
      for (int j = 0; j < 1; j++)
        free(handler2->BLOCKS[i][j]);
    }
    free(handler2->block0->b);
    free(handler2->block0);
    free(handler2);
    return 0;
  }
}

int find2(int *array, int a)
{
  if (array[a] == a)
    return 1;
  else 
    return 0;
}

int count2[BUCKETS_NUM+1] = {0};
int SHT_SecondaryInsertEntry(SHT_info* sht_info, Record record, int block_id)
{
  int a = hash_function2(record.name, handler2->numBuckets);
  handler2->count_records[a]++;
  if (find2(count2, a) == 1 && handler2->BLOCKS[a][1]->b->count_records == 6)
  { 
    SHT_block *b_overflow;
    void* data_overflow;
    Record *rec;
    handler2->BLOCKS[a][1]->b->overflow = malloc(sizeof(SHT_block));
    b_overflow = handler2->BLOCKS[a][1]->b->overflow;
    b_overflow->b = malloc(sizeof(SHT_block_info));
    BF_Block_Init(&b_overflow->block);
    CALL_OR_DIE(BF_AllocateBlock(handler2->fileDesc, b_overflow->block));
    data_overflow = BF_Block_GetData(b_overflow->block);
    rec = data_overflow;
    rec[0] = record;
    BF_Block_SetDirty(b_overflow->block);
    CALL_OR_DIE(BF_UnpinBlock(b_overflow->block));
    b_overflow->b->count_records = 1;
    b_overflow->b->time_created = handler2->count_b;
    handler2->count_b++;
    handler2->BLOCKS[a][1] = b_overflow;
    handler2->count_overflow[a]++;
  }
  if (find2(count2, a) == 0)
  {
    void* data;
    Record* rec;
    count2[a] = a;
   
    BF_Block_Init(&handler2->BLOCKS[a][0]->block);
    BF_AllocateBlock(handler2->fileDesc, handler2->BLOCKS[a][0]->block);
    handler2->BLOCKS[a][0]->b->time_created = handler2->count_b;
    BF_GetBlock(handler2->fileDesc, handler2->BLOCKS[a][0]->b->time_created, handler2->BLOCKS[a][0]->block);
    data = BF_Block_GetData(handler2->BLOCKS[a][0]->block);
    rec = data;
    rec[0] = record;
    BF_Block_SetDirty(handler2->BLOCKS[a][0]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler2->BLOCKS[a][0]->block));
    handler2->BLOCKS[a][0]->b->count_records = 1;
    
    handler2->count_b++;
    handler2->BLOCKS[a][1] = handler2->BLOCKS[a][0];
    return a;
  }
  else if(find2(count2, a) == 1 && handler2->BLOCKS[a][1]->b->count_records < 6 &&  handler2->count_overflow[a] == 0)
  {
    void* data;
    Record *rec;
    int b = handler2->BLOCKS[a][1]->b->count_records;
    CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, handler2->BLOCKS[a][1]->b->time_created, handler2->BLOCKS[a][1]->block));
    data = BF_Block_GetData(handler2->BLOCKS[a][1]->block);
    rec = data;
    rec[b] = record;
    BF_Block_SetDirty(handler2->BLOCKS[a][1]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler2->BLOCKS[a][1]->block));
    handler2->BLOCKS[a][1]->b->count_records++;
  }
  else if(find2(count2, a) == 1 && handler2->BLOCKS[a][1]->b->count_records < 6 &&  handler2->count_overflow[a] != 0)
  {
    void* data;
    Record *rec;
    int b = handler2->BLOCKS[a][1]->b->count_records;
    CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, handler2->BLOCKS[a][1]->b->time_created, handler2->BLOCKS[a][1]->block));
    data = BF_Block_GetData(handler2->BLOCKS[a][1]->block);
    rec = data;
    rec[b] = record;
    BF_Block_SetDirty(handler2->BLOCKS[a][1]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler2->BLOCKS[a][1]->block));
    handler2->BLOCKS[a][1]->b->count_records++;
  }
  return handler2->BLOCKS[a][1]->b->time_created;
}

int SHT_SecondaryGetAllEntries(HT_info* ht_info, SHT_info* sht_info, char* name)
{
  void* data1;
  void* data2;
  Record* rec;
  int counter = 0;
  int a = hash_function2(name, handler2->numBuckets);
  SHT_block* x_block = handler2->BLOCKS[a][0];
  SHT_block* x_block3 = handler2->BLOCKS[a][0]->b->overflow;

  if (handler2->count_overflow[a] == 0)
  {
    counter = 1;
    for (int j = 0; j < x_block->b->count_records; j++)
    {
      CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, x_block->b->time_created, x_block->block));
      data1 = BF_Block_GetData(x_block->block); 
      rec = data1;
      if (strcmp(name, rec[j].name) == 0)
      {
        printf("We have read: %d block\n", counter);
        printRecord(rec[j]);
      }
    }
  }
  else
  { 
    counter++;
    for (int j = 0; j < x_block->b->count_records; j++)
    {
      CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, x_block->b->time_created, x_block->block));
      data1 = BF_Block_GetData(x_block->block); 
      rec = data1;
      if (strcmp(name, rec[j].name) == 0)
      {
        printf("We have read: %d block\n", counter);
        printRecord(rec[j]);
      }
    }
    while (x_block3 != handler2->BLOCKS[a][1])
    {
      counter++;
      for (int j = 0; j < 6; j++)
      {
        CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, x_block3->b->time_created, x_block3->block));
        data2 = BF_Block_GetData(x_block3->block); 
        rec = data2;
        if (strcmp(name, rec[j].name) == 0)
        {
          printf("We have read: %d block\n", counter);
          printRecord(rec[j]);
        }
      }
      x_block3 = x_block3->b->overflow;
    }
    if (x_block3 == handler2->BLOCKS[a][1])
    {
      counter++;
      for (int j = 0; j < 6; j++)
      {
        CALL_OR_DIE(BF_GetBlock(handler2->fileDesc, x_block3->b->time_created, x_block3->block));
        data2 = BF_Block_GetData(x_block3->block); 
        rec = data2;
        if (strcmp(name, rec[j].name) == 0)
        {
          printf("We have read: %d block\n", counter);
          printRecord(rec[j]);
          break;
        }
      }
    }
  }
  return counter;
}