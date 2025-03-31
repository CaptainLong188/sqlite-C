#include "table.h"

uint32_t ROWS_PER_PAGE;
uint32_t TABLE_MAX_ROWS;

Table* newTable()
{
    ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
    TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    
    if (!table)
    {
        fprintf(stderr, "Memory allocation failed (table)\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }

    return table;
}

void freeTable(Table* table)
{
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        if (table->pages[i] != NULL)
        {
            free(table->pages[i]);
            table->pages[i] = NULL;
        }
    }

    free(table);
}

void* rowSlot(Table* table, uint32_t rowNum)
{
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
    
    void* page = (table -> pages)[pageNum];

    if (page == NULL) // Allocate memory when we try to access the page for the first time
    {
        table->pages[pageNum] = malloc(PAGE_SIZE);
        // printf("A new page was allocated\n");
    }
    
    page = table->pages[pageNum];
    uint32_t byteOffset = rowOffset * ROW_SIZE;

    return (char*)page + byteOffset;
}