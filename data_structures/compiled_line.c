#include "compiled_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Compiled_Line *create_compiled_line(int line_index, unsigned int *begin_address)
{
    Compiled_Line *compiled_inst = (Compiled_Line *)calloc(1, sizeof(Compiled_Line));
    if (compiled_inst == NULL)
    {
        perror("Allocation failed for a new compiled_inst");
        exit(EXIT_FAILURE);
    }

    compiled_inst->line_index = line_index;
    compiled_inst->words = (unsigned int *)calloc(INITIAL_CAPACITY, sizeof(unsigned int));
    if (compiled_inst->words == NULL)
    {
        perror("Allocation failed for words array");
        free(compiled_inst);
        exit(EXIT_FAILURE);
    }
    compiled_inst->num_of_words = 0;
    compiled_inst->capacity = INITIAL_CAPACITY;
    compiled_inst->begin_address = *begin_address;
    compiled_inst->end_address = compiled_inst->begin_address;
    memset(compiled_inst->missing_label, '\0', sizeof(compiled_inst->missing_label));
    compiled_inst->next_compiled_line = NULL;

    return compiled_inst;
}

Compiled_Line *insert_compiled_line_to_table(Compiled_Line *compiled_line, int line_index, unsigned int *begin_address)
{
    Compiled_Line *current = compiled_line;
    if (compiled_line == NULL) 
    {
        return create_compiled_line(line_index, begin_address);
    }
    
    while (current->next_compiled_line != NULL)
    {
        current = current->next_compiled_line;
    }
    
    current->next_compiled_line = create_compiled_line(line_index, begin_address);
    return compiled_line;
}

Compiled_Line *get_compiled_line(Compiled_Line *compiled_line, int line_index)
{
    while (compiled_line != NULL)
    {
        if (compiled_line->line_index == line_index)
        {
            return compiled_line;
        }
        compiled_line = compiled_line->next_compiled_line;
    }
    return NULL;
}

void insert_word(Compiled_Line *compiled_line, unsigned int word, unsigned int *address)
{
    if (compiled_line->num_of_words >= compiled_line->capacity)
    {
        compiled_line->capacity *= 2;
        compiled_line->words = (unsigned int *)realloc(compiled_line->words, compiled_line->capacity * sizeof(unsigned int));
        if (compiled_line->words == NULL)
        {
            perror("Memory reallocation failed for words array");
            exit(EXIT_FAILURE);
        }
    }
    compiled_line->words[compiled_line->num_of_words++] = word;
    compiled_line->end_address = *address;
    (*address)++;
}

void free_compiled_line(Compiled_Line *compiled_line) 
{
    free(compiled_line->words);
    free(compiled_line);
}

void free_compiled_line_table(Compiled_Line *compiled_line) 
{
    while (compiled_line != NULL) 
    {
        Compiled_Line *temp = compiled_line;
        compiled_line = compiled_line->next_compiled_line;
        free_compiled_line(temp);
    }
} 
