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
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
}Table;

Table* newTable(void);
void freeTable(Table* table);

/* Calculate the address where we will read or write */
void* rowSlot(Table* table, uint32_t row);

#endif