#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ht_file.h"
#include "record.h"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int hash_function(int i, int base)
{
  return (i % base + 1);
}

int HT_CreateFile(char *fileName, int buckets)
{
  handler = malloc(sizeof(HT_info));
  handler->block0 = malloc(sizeof(HT_block));
  handler->block0->b = malloc(sizeof(HT_block_info));
  handler->numBuckets = buckets;
  for (int i = 0; i <= handler->numBuckets; i++)
  {
    handler->BLOCKS[i][0] = malloc(sizeof(HT_block));
  }
  for (int i = 0; i <= handler->numBuckets; i++)
  {
    handler->BLOCKS[i][0]->b = malloc(sizeof(HT_block_info));
  }
  handler->flag = 1;
  handler->fileDesc = 0;
  handler->count_b = 0;
  for (int i = 0; i <= handler->numBuckets; i++)
  {
    handler->count_overflow[i] = 0;
  }
  
  CALL_OR_DIE(BF_CreateFile(fileName));
  return 0;
}

HT_info* HT_OpenFile(char *fileName)
{
  handler->fileDesc++;
  BF_ErrorCode code = BF_OpenFile(fileName, &(handler->fileDesc));

  BF_Block_Init(&handler->block0->block);                                        
  CALL_OR_DIE(BF_AllocateBlock(handler->fileDesc, handler->block0->block));      
  handler->count_b++;

  if (code != BF_OK || handler->flag != 1)
    return NULL;
}

int HT_CloseFile(HT_info* ht_info)
{
  BF_ErrorCode code = BF_CloseFile(handler->fileDesc);

  if (code != BF_OK)
    return -1;
  else 
  {
    for (int i = 0; i <= handler->numBuckets; i++)
    {
      free(handler->BLOCKS[i][0]->b);
    }
    for (int i = 0; i <= handler->numBuckets; i++)
    {
      for (int j = 0; j < 1; j++)
        free(handler->BLOCKS[i][j]);
    }
    free(handler->block0->b);
    free(handler->block0);
    free(handler);
    return 0;
  }
}

int find(int *array, int a)
{
  if (array[a] == a)
    return 1;
  else 
    return 0;
}

int count[BUCKETS_NUM+1] = {0};
int HT_InsertEntry(HT_info* ht_info, Record record)
{   
  int a = hash_function(record.id, handler->numBuckets);
  if (find(count, a) == 1 && handler->BLOCKS[a][1]->b->count_records == 6)
  { 
    HT_block *b_overflow;
    void* data_overflow;
    Record *rec;
    handler->BLOCKS[a][1]->b->overflow = malloc(sizeof(HT_block));
    b_overflow = handler->BLOCKS[a][1]->b->overflow;
    b_overflow->b = malloc(sizeof(HT_block_info));
    BF_Block_Init(&b_overflow->block);
    CALL_OR_DIE(BF_AllocateBlock(handler->fileDesc, b_overflow->block));
    data_overflow = BF_Block_GetData(b_overflow->block);
    rec = data_overflow;
    rec[0] = record;
    BF_Block_SetDirty(b_overflow->block);
    CALL_OR_DIE(BF_UnpinBlock(b_overflow->block));
    b_overflow->b->count_records = 1;
    b_overflow->b->time_created = handler->count_b;
    handler->count_b++;
    handler->BLOCKS[a][1] = b_overflow;
    handler->count_overflow[a]++;
  }
  else if (find(count, a) == 0)
  {
    void* data;
    Record *rec;
    
    count[a] = a;
    BF_Block_Init(&handler->BLOCKS[a][0]->block);
    CALL_OR_DIE(BF_AllocateBlock(handler->fileDesc, handler->BLOCKS[a][0]->block));
    data = BF_Block_GetData(handler->BLOCKS[a][0]->block);
    rec = data;
    rec[0] = record;
    BF_Block_SetDirty(handler->BLOCKS[a][0]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler->BLOCKS[a][0]->block));
    handler->BLOCKS[a][0]->b->count_records = 1;
    
    handler->count_b++;
    handler->BLOCKS[a][1] = handler->BLOCKS[a][0];
    return a;
  }
  else if(find(count, a) == 1 && handler->BLOCKS[a][1]->b->count_records < 6 &&  handler->count_overflow[a] == 0)
  {
    void* data;
    Record *rec;
    int b = handler->BLOCKS[a][1]->b->count_records;
    CALL_OR_DIE(BF_GetBlock(handler->fileDesc,a, handler->BLOCKS[a][1]->block));
    data = BF_Block_GetData(handler->BLOCKS[a][1]->block);
    rec = data;
    rec[b] = record;
    BF_Block_SetDirty(handler->BLOCKS[a][1]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler->BLOCKS[a][1]->block));
    handler->BLOCKS[a][1]->b->count_records++;
  }
  else if(find(count, a) == 1 && handler->BLOCKS[a][1]->b->count_records < 6 &&  handler->count_overflow[a] != 0)
  {
    void* data;
    Record *rec;
    int b = handler->BLOCKS[a][1]->b->count_records;
    CALL_OR_DIE(BF_GetBlock(handler->fileDesc,handler->BLOCKS[a][1]->b->time_created, handler->BLOCKS[a][1]->block));
    data = BF_Block_GetData(handler->BLOCKS[a][1]->block);
    rec = data;
    rec[b] = record;
    BF_Block_SetDirty(handler->BLOCKS[a][1]->block);
    CALL_OR_DIE(BF_UnpinBlock(handler->BLOCKS[a][1]->block));
    handler->BLOCKS[a][1]->b->count_records++;
  }

  return handler->BLOCKS[a][1]->b->time_created;
}

int HT_GetAllEntries(HT_info* ht_info, int value)
{
  void* data1;
  void* data2;
  Record* rec;
  int flag = 0;
  int counter = 0;
  int a = hash_function(value, handler->numBuckets);
  HT_block* x_block = handler->BLOCKS[a][0];
  HT_block* x_block3 = handler->BLOCKS[a][0]->b->overflow;

  if (value < handler->numBuckets*6)
  {
    counter++;
    for (int j = 0; j < x_block->b->count_records; j++)
    {
      CALL_OR_DIE(BF_GetBlock(handler->fileDesc, a, x_block->block));
      data1 = BF_Block_GetData(x_block->block); 
      rec = data1;
      if (rec[j].id == value)
      {
        printf("We have read: %d block\n", counter);
        printRecord(rec[j]);
        break;
      }
    }
  }
  else
  {  
    while (x_block3 != handler->BLOCKS[a][1])
    {
      counter++;
      for (int j = 0; j < 6; j++)
      {
        CALL_OR_DIE(BF_GetBlock(handler->fileDesc, x_block3->b->time_created, x_block3->block));
        data2 = BF_Block_GetData(x_block3->block); 
        rec = data2;
        if (rec[j].id == value)
        {
          printf("We have read: %d block\n", counter);
          printRecord(rec[j]);
          flag = 1;
          break;
        }
      }
      if (flag == 1)
      {
        break;
      }
      x_block3 = x_block3->b->overflow;
    }
  }
  if (x_block3 == handler->BLOCKS[a][1])
  {
    counter++;
    for (int j = 0; j < 6; j++)
    {
      CALL_OR_DIE(BF_GetBlock(handler->fileDesc, x_block3->b->time_created, x_block3->block));
      data2 = BF_Block_GetData(x_block3->block); 
      rec = data2;
      if (rec[j].id == value)
      {
        printf("We have read: %d block\n", counter);
        printRecord(rec[j]);
        flag = 1;
        break;
      }
    }
  }
}