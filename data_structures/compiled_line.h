#ifndef COMPILED_LINE_H
#define COMPILED_LINE_H

#include "../helpers/utils.h"

/* "Instruction" / "Directive" Compiled Line */
typedef struct Compiled_Line /* Code section */
{
    int line_index;
    unsigned int *words; /* Pointer to an array of words (15-bit) */
    int num_of_words; /* Number of words in the array */
    unsigned int begin_address; /* Starting address of the compiled line */
    unsigned int end_address; /* Ending address of the compiled line */
    enum
    {
        src_label, /* Source label */
        dest_label /* Destination label */
    } missing_label_op_type[2]; /* Type of missing labels (source/destination) */
    char missing_label[2][MAX_LABEL_LENGTH]; /* Labels that need to be resolved */
    struct Compiled_Line *next_compiled_line; /* Pointer to the next compiled line */

} Compiled_Line;

/**
 * @brief Create a new "compiled instruction" / "compile directive" node.
 * 
 * @param line_index The line index from the am_file.
 * @param begin_address The starting address for the new compiled line.
 * @return Pointer to the newly created Compiled_Line node.
 */
Compiled_Line *create_compiled_line(int line_index, unsigned int *begin_address);

/**
 * @brief Inserts a new compiled_line with the given line_index into the compiled_line table.
 * 
 * @param compiled_line Pointer to the head of the compiled_line table.
 * @param line_index The line index from the am_file.
 * @param begin_address The starting address for the new compiled line.
 * @return Pointer to the updated compiled_line table.
 */
Compiled_Line *insert_compiled_line_to_table(Compiled_Line *compiled_line, int line_index, unsigned int *begin_address);

/**
 * @brief Searches for a Compiled_Line with the specified line_index in the compiled_line table.
 * 
 * @param compiled_line Pointer to the head of the compiled_line table.
 * @param line_index The line index to search for.
 * @return Pointer to the Compiled_Line node with the specified line_index, or NULL if not found.
 */
Compiled_Line *get_compiled_line(Compiled_Line *compiled_line, int line_index);

/**
 * @brief Inserts a new word into the words array of the given Compiled_Line.
 * 
 * @param compiled_line Pointer to the Compiled_Line node where the word will be added.
 * @param word The word to add.
 * @param address Pointer to the address for the new word.
 */
void insert_word(Compiled_Line *compiled_line, unsigned int word, unsigned int *address);

/**
 * @brief Frees the memory allocated for the given Compiled_Line and its associated words.
 * 
 * @param compiled_line Pointer to the Compiled_Line node to be freed.
 */
void free_compiled_line(Compiled_Line *compiled_line);

/**
 * @brief Frees the entire compiled_line table and all the compiled_line nodes it contains.
 * 
 * @param compiled_line Pointer to the head of the compiled_line table.
 */
void free_compiled_line_table(Compiled_Line *compiled_line);

#endif /* COMPILED_LINE_H */
