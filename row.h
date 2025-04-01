#ifndef ROW_H
#define ROW_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE    255

typedef struct 
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

#define ID_OFFSET           0
#define ID_SIZE             sizeof(((Row*)0)->id)

#define USERNAME_OFFSET     (ID_OFFSET + ID_SIZE)
#define USERNAME_SIZE       sizeof(((Row*)0)->username)

#define EMAIL_OFFSET        (USERNAME_OFFSET + USERNAME_SIZE)
#define EMAIL_SIZE          sizeof(((Row*)0)->email)

#define ROW_SIZE            (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

/* Converts a Row struct into compact binary format*/
void serializeRow(Row* source, char* destination);

/* Converts the compact binary format back into a Row struct*/
void deserializeRow(char* source, Row* destination);

/* Prints the data in the row, for now id -- username --email*/
void printRow(Row* row);

#endif