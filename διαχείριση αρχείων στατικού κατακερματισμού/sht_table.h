#ifndef SHT_TABLE_H
#define SHT_TABLE_H
#include <record.h>
#include <ht_table.h>

typedef struct sht_block_info SHT_block_info;
typedef struct sht_block SHT_block;

struct sht_block_info
{
    SHT_block *overflow;
    int count_records;
    int time_created;
};

struct sht_block        
{
    BF_Block *block;
    SHT_block_info *b;
};

typedef struct 
{
    SHT_block *block0;
    int fileDesc;
    int count_b;
    int numBuckets;
    int count_records[BUCKETS_NUM+1];
    SHT_block *BLOCKS[BUCKETS_NUM+1][2];
    int count_overflow[BUCKETS_NUM+1];
} SHT_info;
SHT_info *handler2;

int SHT_CreateSecondaryIndex(char *sfileName, int buckets, char* fileName);
SHT_info* SHT_OpenSecondaryIndex(char *sfileName);
int SHT_CloseSecondaryIndex(SHT_info* header_info);
int SHT_SecondaryInsertEntry(SHT_info* header_info, Record record, int block_id);
int SHT_SecondaryGetAllEntries(HT_info* ht_info, SHT_info* header_info, char* name);
void HashStatistics(char* filename);

#endif // SHT_FILE_H