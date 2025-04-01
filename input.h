#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char* buffer;
    size_t buffer_length;
    size_t input_length;
}InputBuffer;

InputBuffer* newInputBuffer(void);
/*Stores the read line in the buffer and the size of the allocated buffer*/
int getline(char** lineptr, size_t* n, FILE* stream);
void readInputData(InputBuffer* input_buffer);
void closeInputBuffer(InputBuffer* input_buffer);

#endif