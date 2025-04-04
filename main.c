#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "input.h"
#include "row.h"
#include "table.h"

void printPrompt()
{
    printf("db > ");
}

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
}StatementType;

typedef struct
{
    StatementType type;
    Row row_to_insert; // Only used by insert statement
}Statement;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG
}PreparateResult;

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
}ExecuteResult;

MetaCommandResult doMetaCommand(InputBuffer* input_buffer, Table* table)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        closeInputBuffer(input_buffer);
        dbClose(table);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(input_buffer->buffer, ".help") == 0)
    {
        printf("We're are working on it\n");
        return META_COMMAND_SUCCESS;
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED;
    }
}

PreparateResult prepareStatement(InputBuffer* input_buffer, Statement* statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        const char* del = " ";

        char* keyword = strtok(input_buffer->buffer, del); // For now unused
        char* id_string = strtok(NULL, del);
        char* username = strtok(NULL, del);
        char* email = strtok(NULL, del);

        if (keyword == NULL || id_string == NULL || username == NULL || email == NULL)
        {
            return PREPARE_SYNTAX_ERROR;
        }

        int id = atoi(id_string);

        if (id < 0)
        {
            return PREPARE_NEGATIVE_ID;
        }

        if (strlen(username) > COLUMN_USERNAME_SIZE ||  strlen(email) > COLUMN_EMAIL_SIZE)
        {
            return PREPARE_STRING_TOO_LONG;
        }

        statement->row_to_insert.id = id;
        strcpy(statement->row_to_insert.username, username);
        strcpy(statement->row_to_insert.email, email);

        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult executeInsert(Statement* statement, Table* table)
{
    // Check if the table is not full
    if(table->num_rows >= TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }

    Row* rowToInsert = &(statement->row_to_insert); // the statement will provide the values to be inserted
    //printf("(%d - %s - %s)\n", rowToInsert->id, rowToInsert->username, rowToInsert->email);

    serializeRow(rowToInsert, rowSlot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}


ExecuteResult executeSelect(Statement* statement, Table* table)
{
    Row row;

    for (uint32_t i = 0; i < table->num_rows; i++)
    {
        deserializeRow(rowSlot(table, i), &row);
        printRow(&row);
    }

    return EXECUTE_SUCCESS;
}

ExecuteResult executeStatement(Statement* statement, Table* table)
{
    switch (statement->type)
    {
        case STATEMENT_INSERT:
            return executeInsert(statement, table);
        
        case STATEMENT_SELECT:
            return executeSelect(statement, table);
    }

    return EXECUTE_SUCCESS;
}

int main(int argc, char* argv[])
{
    
    if (argc < 2)
    {
        fprintf(stderr, "Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    InputBuffer* input_buffer = newInputBuffer();
    Table* table = dbOpen(filename);

    while (true)
    {
        printPrompt();
        readInputData(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (doMetaCommand(input_buffer, table))
            {
                case META_COMMAND_SUCCESS:
                    continue;
                    
                case META_COMMAND_UNRECOGNIZED: 
                    printf("Unrecognized command : %s\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepareStatement(input_buffer, &statement))
        {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
            case PREPARE_NEGATIVE_ID:
                printf("Error: ID must be positive.\n");
                continue;
            case PREPARE_STRING_TOO_LONG:
                printf("Error : String is too long.\n");
                continue;
        }

        switch (executeStatement(&statement, table))
        {
            case EXECUTE_SUCCESS:
                printf("Statement executed successfully.\n");
                break;
            
            case EXECUTE_TABLE_FULL:
                printf("Error: Table full.\n");
                break;
        }
    }

    return EXIT_SUCCESS;
}