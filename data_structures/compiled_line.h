#ifndef COMPILED_LINE_H
#define COMPILED_LINE_H

#include "../helpers/utils.h"

/* Initial capacity for the words array to reduce frequent reallocations */
#define INITIAL_CAPACITY 10

/* Struct representing a compiled line of assembly code */
typedef struct Compiled_Line {
    int line_index;                  /* Index of the line in the original source file */
    unsigned int *words;             /* Dynamic array to store encoded words */
    int num_of_words;                /* Current number of words in the array */
    int capacity;                    /* Current capacity of the words array */
    unsigned int begin_address;      /* Starting address of this compiled line */
    unsigned int end_address;        /* Ending address of this compiled line */
    enum {
        src_label,                   /* Label used as a source operand */
        dest_label                   /* Label used as a destination operand */
    } missing_label_op_type[2];      /* Type of missing labels (up to 2 per line) */
    char missing_label[2][MAX_LABEL_LENGTH];  /* Names of missing labels */
    struct Compiled_Line *next_compiled_line; /* Pointer to the next compiled line */
} Compiled_Line;

/* Function to create a new Compiled_Line structure */
Compiled_Line *create_compiled_line(int line_index, unsigned int *begin_address);

/* Function to insert a new Compiled_Line into the linked list of compiled lines */
Compiled_Line *insert_compiled_line_to_table(Compiled_Line *compiled_line, int line_index, unsigned int *begin_address);

/* Function to retrieve a Compiled_Line by its line index */
Compiled_Line *get_compiled_line(Compiled_Line *compiled_line, int line_index);

/* Function to insert an encoded word into a Compiled_Line */
void insert_word(Compiled_Line *compiled_line, unsigned int word, unsigned int *address);

/* Function to free the memory allocated for a single Compiled_Line */
void free_compiled_line(Compiled_Line *compiled_line);

/* Function to free the entire linked list of Compiled_Line structures */
void free_compiled_line_table(Compiled_Line *compiled_line);

#endif /* COMPILED_LINE_H */ 
