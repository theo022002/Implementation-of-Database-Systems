#ifndef HP_FILE_H
#define HP_FILE_H
#include <record.h>
#include "bf.h"

/* Η δομή HP_info κρατάει μεταδεδομένα που σχετίζονται με το αρχείο σωρού*/

typedef struct hp_block_info HP_block_info;
typedef struct hp_block HP_block;

struct  hp_block_info        
{
    HP_block *next;
    int count_records;
};

struct hp_block        
{
    BF_Block *block;
    HP_block_info *b;
};

typedef struct 
{   
    HP_block *block0;       //deikths se prwto block
    HP_block *last_entry;
    int fileDesc;
    int count_b;
    int flag;
} HP_info;
HP_info *handler;

int HP_CreateFile(char *fileName);
HP_info* HP_OpenFile(char *fileName);
int HP_CloseFile(HP_info* header_info);
int HP_InsertEntry(HP_info* header_info, Record record);
int HP_GetAllEntries(HP_info* header_info, int id);

#endif // HP_FILE_H