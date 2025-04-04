#include "input.h"

InputBuffer* newInputBuffer(void)
{
    InputBuffer* inputBuffer = (InputBuffer*)malloc(sizeof(InputBuffer));

    if (!inputBuffer)
    {
        fprintf(stderr, "Memory allocation failed(input).\n");
        exit(EXIT_FAILURE);
    }

    inputBuffer->buffer = NULL;
    inputBuffer->buffer_length = 0;
    inputBuffer->input_length = 0;

    return inputBuffer;
}

int getline(char** lineptr, size_t* n, FILE* stream)
{
    if (lineptr == NULL || n == NULL || stream == NULL)
    {
        return -1;
    }

    const int INITIAL_BUFFER_SIZE = 128;

    if (*lineptr == NULL || *n == 0)
    {
        *n = INITIAL_BUFFER_SIZE; // Initial buffer size
        *lineptr = (char*) malloc(INITIAL_BUFFER_SIZE);

        if (lineptr == NULL) 
        {
            return -1;
        }
    }

    size_t pos = 0;
    int ch; // Using integer type to handle EOF

    while ((ch = fgetc(stream)) != EOF)
    {
        // Resize buffer if needed
        if (pos + 1 >= *n)
        {
            (*n) *= 2;
            char* temp = (char*) realloc(*lineptr, *n); // Prevents losing the original pointer
            
            if (temp == NULL)
            {
                return -1;
            }
            
            *lineptr = temp;
        }

        (*lineptr)[pos++] = ch;

        if (ch == '\n') { break; }
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

void readInputData(InputBuffer* input_buffer)
{
    int bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    
    if (bytes_read == -1)
    {
        fprintf(stderr, "Error reading input\n");
        exit(EXIT_FAILURE);
    }
    
    // Ignore trailing newline
    input_buffer->input_length = (size_t)bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';
}

void closeInputBuffer(InputBuffer* input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}
