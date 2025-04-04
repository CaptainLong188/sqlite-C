#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdlib.h>
#include "row.h"

// Pages won't exist next to each other in memory
#define TABLE_MAX_PAGES     100
#define PAGE_SIZE           4096

extern uint32_t ROWS_PER_PAGE;
extern uint32_t TABLE_MAX_ROWS;

typedef struct
{
    FILE* file;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
}Pager;

typedef struct
{
    uint32_t num_rows;
    Pager* pager;
}Table;

/* Opens the database file, initialize pager and table*/
Table* dbOpen(const char* filename);

/* Calculate the address where we will read or write */
void* rowSlot(Table* table, uint32_t row);

/* Opens the database file and keeps track of its size*/
Pager* pagerOpen(const char* filename);

/* Fetch a specific page*/
void* getPage(Pager* pager, uint32_t page_num);

/* Flush the pages to disk*/
void pagerFlush(Pager* pager, uint32_t page_num, uint32_t size);

/* Flushes the page cache to disk, close the database file
   free memory for the Pager and Table*/
void dbClose(Table* table);

#endif