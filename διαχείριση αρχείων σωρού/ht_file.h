#ifndef HT_FILE_H
#define HT_FILE_H
#include <record.h>
#include "bf.h"
#define BUCKETS_NUM 5

typedef struct ht_block_info HT_block_info;
typedef struct ht_block HT_block;

struct  ht_block_info        
{
    HT_block *overflow;
    int count_records;
    int time_created;
};

struct ht_block        
{
    BF_Block *block;
    HT_block_info *b;
};

typedef struct 
{   
    HT_block *block0;  //deikths se prwto block
    int fileDesc;
    int count_b;
    int flag;
    int count_overflow[BUCKETS_NUM+1];
    long int numBuckets;
    HT_block *BLOCKS[BUCKETS_NUM+1][2];
} HT_info;
HT_info *handler;

int HT_CreateFile(char *fileName, int buckets);
HT_info* HT_OpenFile(char *fileName);
int HT_CloseFile(HT_info* header_info);
int HT_InsertEntry(HT_info* header_info, Record record);
int HT_GetAllEntries(HT_info* header_info, int id);

#endif // HT_FILE_H
