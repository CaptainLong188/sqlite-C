#include "table.h"

uint32_t ROWS_PER_PAGE;
uint32_t TABLE_MAX_ROWS;

Table* dbOpen(const char* filename)
{
    ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
    TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

    Pager* pager = pagerOpen(filename);
    printf("The size of the db file is : %d bytes.\n", pager->file_length);
    
    uint32_t num_pages = (pager->file_length) / PAGE_SIZE;
    uint32_t num_additional_rows = ((pager->file_length) % PAGE_SIZE) / ROW_SIZE; 
    printf("Number of pages and additional rows : %d %d\n", num_pages, num_additional_rows);

    Table* table = (Table*)malloc(sizeof(Table));
    
    if (!table)
    {
        fprintf(stderr, "Memory allocation failed (table)\n");
        exit(EXIT_FAILURE);
    }
    
    table->pager = pager;
    table->num_rows = num_pages * ROWS_PER_PAGE + num_additional_rows;
    printf("The current number of rows is: %d.\n", table->num_rows);

    return table;
}

Pager* pagerOpen(const char* filename)
{
    FILE* file = fopen(filename, "rb+");

    if (!file)
    {
        file = fopen(filename, "wb+"); // Create the file if it doesn't exit;

        if (!file)
        {
            fprintf(stderr, "Unable to open file\n");
            exit(EXIT_FAILURE);
        }
    }
    
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);

    if (file_size == -1L)
    {
        fprintf(stderr, "Ftell failed.\n");
        exit(EXIT_FAILURE);
    }

    Pager* pager = (Pager*)malloc(sizeof(Pager));

    if (!pager)
    {
        fprintf(stderr, "Memory allocation failed (pager)\n");
        exit(EXIT_FAILURE);
    }

    pager->file = file;
    pager->file_length = (uint32_t)file_size;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pager->pages[i] = NULL;
    }

    return pager;
}

void* rowSlot(Table* table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = getPage(table->pager, page_num);
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    return (char*)page + byte_offset;
}

// Pages are saved next to each other in the database file
// Page 0 at offset 0, page 1 at offset 4096, etc.  
void* getPage(Pager* pager, uint32_t page_num)
{
    if (page_num >= TABLE_MAX_PAGES)
    {
        fprintf(stderr, "Tried to fetch page number out of bounds. %d > %d.\n", page_num + 1, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    // Cache miss. Alocate memory and load from file if possible
    if (pager->pages[page_num] == NULL)
    {
        void* page = malloc(PAGE_SIZE);
        
        if (!page)
        {
            fprintf(stderr, "Memory allocation failed (page)\n");
            exit(EXIT_FAILURE);
        }

        uint32_t num_pages = (pager->file_length) / PAGE_SIZE;
        
        if ((pager->file_length) % PAGE_SIZE) { num_pages += 1; } // Ceil
        printf("The number of pages is %d and the current page is %d.\n", num_pages, page_num);
        
        // If page already exits fetch its data, if not just return a blank plage
        if (page_num < num_pages)
        {
            printf("Page already exits, fetching data ...\n");
            fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
            clearerr(pager->file); // Reset error state before reading
            fread(page, 1, PAGE_SIZE, pager->file);

            if (ferror(pager->file))
            {
                fprintf(stderr, "Error reading from db file.\n");
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void pagerFlush(Pager* pager, uint32_t page_num, uint32_t size)
{
    if (pager->pages[page_num] == NULL)
    {
        printf("Tried to flush null page.\n");
        exit(EXIT_FAILURE);
    }

    printf("The size in bytes is %d.\n", size);

    fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
    size_t bytes_written = fwrite(pager->pages[page_num], 1, size, pager->file);

    printf("Number of bytes written to disk is : %lu.\n", (unsigned long) bytes_written);

    if (bytes_written != size)
    {
        fprintf(stderr, "Error writing to db file : Only %d bytes of %d were written.\n", bytes_written, size);
        exit(EXIT_FAILURE);
    }
}

void dbClose(Table* table)
{
    Pager* pager = table->pager;
    uint32_t num_full_pages = (table->num_rows) / ROWS_PER_PAGE;
    printf("The number of full pages is : %d.\n", num_full_pages);

    for (uint32_t i = 0; i < num_full_pages; i++)
    {
        if (pager->pages[i] == NULL) {continue;}

        pagerFlush(pager, i, PAGE_SIZE); // Save to disk the desired page
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // Handle partial page
    uint32_t num_additional_rows = (table->num_rows) % ROWS_PER_PAGE;
    printf("The number of additional rows is : %d.\n", num_additional_rows);

    if (num_additional_rows > 0)
    {
        uint32_t last_page = num_full_pages;
        
        if (pager->pages[last_page] != NULL)
        {
            printf("Flushing...\n");
            pagerFlush(pager, last_page, num_additional_rows * ROW_SIZE);
            free(pager->pages[last_page]);
            pager->pages[last_page] = NULL;
        }
    }

    int result_close = fclose(pager->file);

    if (result_close == EOF)
    {
        fprintf(stderr, "Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    // Free the pages that weren't writte to disk
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        if (pager->pages[i] != NULL)
        {
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}