#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "ht_table.h"
#include "sht_table.h"

void HashStatistics(char* filename)
{
    if (strcmp(filename, "data.db") == 0)
    {
      int count_over = 0;
      int count_buckets[BUCKETS_NUM+1] = {0};
      int count_rec[BUCKETS_NUM+1] = {0};
      printf("\n\nFor the hash file.");
      printf("\nThe blocks in the hash file are: %d.\n", handler->count_b);
      for (int i = 1; i <= handler->numBuckets; i++)
      {
        count_buckets[i] = handler->count_overflow[i] + 1;
        printf("The bucket: %d has %d blocks and %d overflow blocks.\n", i, count_buckets[i], handler->count_overflow[i]);
        if (handler->count_overflow[i] != 0)
        {
          count_over++;
          //printf("The bucket %d has %d overflow block.\n", i, handler->count_overflow[i]);
        }
        count_rec[i] = handler->count_records[i];
      }
      int MAX = count_rec[0];
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        if(count_rec[i] >= MAX)
          MAX = count_rec[i];
      }
      int MIN = count_rec[1];
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        if(count_rec[i] < MIN)
          MIN = count_rec[i];
      }
      int sum = 0;
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        sum = sum + count_rec[i];
      }
      sum = sum / BUCKETS_NUM;
      printf("\nThe average number of records in a bucket is: %d.\n", sum);
      printf("The min number of records in a bucket is: %d.\n", MIN);
      printf("The max number of records in a bucket is: %d.\n", MAX);
      printf("There are %d buckets with overflow blocks.\n", count_over);
    }
    else if (strcmp(filename, "index.db") == 0)
    {
      int count_over = 0;
      int count_buckets[BUCKETS_NUM+1] = {0};
      int count_rec[BUCKETS_NUM+1] = {0};
      printf("\n\nFor the secondary hash file.");
      printf("\nThe blocks in the file are: %d.\n", handler2->count_b);
      for (int i = 1; i <= handler2->numBuckets; i++)
      {
        count_buckets[i] = handler2->count_overflow[i] + 1;
        printf("The bucket: %d has %d blocks and %d overflow blocks.\n", i, count_buckets[i], handler2->count_overflow[i]);
        if (handler2->count_overflow[i] != 0)
        {
          count_over++;
          //printf("The bucket %d has %d overflow block.\n", i, handler2->count_overflow[i]);
        }
        count_rec[i] = handler2->count_records[i];
      }
      int MAX = count_rec[0];
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        if(count_rec[i] >= MAX)
          MAX = count_rec[i];
      }
      int MIN = count_rec[1];
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        if(count_rec[i] < MIN)
          MIN = count_rec[i];
      }
      int sum = 0;
      for(int i = 1; i <= BUCKETS_NUM; i++)
      {
        sum = sum + count_rec[i];
      }
      sum = sum / BUCKETS_NUM;
      printf("\nThe average number of records in a bucket is: %d.\n", sum);
      printf("The min number of records in a bucket is: %d.\n", MIN);
      printf("The max number of records in a bucket is: %d.\n", MAX);
      printf("There are %d buckets with overflow blocks.\n", count_over);
    }
    return;
}