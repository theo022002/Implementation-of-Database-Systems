#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int HP_CreateFile(char *fileName)
{
  handler = malloc(sizeof(HP_info));
  handler->block0 = malloc(sizeof(HP_block));
  handler->block0->b = malloc(sizeof(HP_block_info));
  handler->fileDesc = 0;
  handler->count_b = 0;
  handler->flag = 0;

  CALL_OR_DIE(BF_CreateFile(fileName));
}

HP_info* HP_OpenFile(char *fileName)
{
  handler->fileDesc++;
  BF_ErrorCode code = BF_OpenFile(fileName, &(handler->fileDesc));

  BF_Block_Init(&handler->block0->block);                                        //arxikopoioume block
  CALL_OR_DIE(BF_AllocateBlock(handler->fileDesc, handler->block0->block));      //desmeuoume mnhmh gia block0
  handler->last_entry = handler->block0;
  handler->block0->b->count_records = 6;
  handler->count_b++;

  if (code != BF_OK || handler->flag != 0)
    return NULL;
}

int HP_CloseFile(HP_info* hp_info)
{
  BF_ErrorCode code = BF_CloseFile(handler->fileDesc);

  if (code != BF_OK)
    return -1;
  else 
  {
    free(handler->block0);
    free(handler);
    return 0;
  }
}

void* data;
int HP_InsertEntry(HP_info* hp_info, Record record)
{   
  if (handler->last_entry->b->count_records < 6)
  {
    Record *rec;
    int a = handler->last_entry->b->count_records;
    rec = data;
    rec[a] = record;
    BF_Block_SetDirty(handler->last_entry->block);
    CALL_OR_DIE(BF_UnpinBlock(handler->last_entry->block));
    handler->last_entry->b->count_records++;
  }

  else if(handler->last_entry->b->count_records == 6)
  {
    Record *rec;
    handler->last_entry->b->next = malloc(sizeof(HP_block));
    HP_block *new;
    new = handler->last_entry->b->next;
    BF_Block_Init(&new->block);
    CALL_OR_DIE(BF_AllocateBlock(handler->fileDesc, new->block));
    data = BF_Block_GetData(new->block);
    rec = data;
    rec[0] = record;
    BF_Block_SetDirty(new->block);
    CALL_OR_DIE(BF_UnpinBlock(new->block));
    handler->last_entry = new;
    new->b = malloc(sizeof(HP_block_info));
    new->b->count_records = 1;
    handler->count_b++;
  }
  
  return (handler->count_b);
}

int HP_GetAllEntries(HP_info* hp_info, int value)
{    
  void *data1;
  Record* rec;
  HP_block* x_block;
  x_block = handler->block0->b->next;
  int flag = 0;
  int read_blocks = 0;
  
  for (int i = 1; i < handler->count_b; i++)
  {
    for (int j = 0; j < x_block->b->count_records; j++)
    {
      CALL_OR_DIE(BF_GetBlock(handler->fileDesc, i, x_block->block));
      data1 = BF_Block_GetData(x_block->block); 
      rec = data1;
      if (rec[j].id == value)
      {
        printf("The record is in the block: %d\n", i);
        printRecord(rec[j]);
        flag = 1;
        read_blocks = i;
        break;
      }
    }
    if (flag == 1)
    {
      break;
    }
    x_block = x_block->b->next;
  }
  return read_blocks;  
}