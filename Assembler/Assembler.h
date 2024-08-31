#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../helpers/utils.h"
#include "../analyzer/analyzer.h"
#include "../helpers/analyzer_helper.h"
#include "../data_structures/compiled_line.h"
#include "../data_structures/symbol_table.h"
#include <stdio.h>

#define DEST_INDENTATION 2
#define SRC_INDENTATION 9
#define OPCODE_INDENTATION 5
#define DEST_REG_INDENTATION 2
#define SRC_REG_INDENTATION 7

#define MEMORY_SIZE 4096
#define BASE_ADDRESS 100

#define MAX_FILE_NAME_LENGTH 256

typedef struct Object_File 
{
    Compiled_Line * data_section;    /* Compiled lines for data section */
    Compiled_Line * code_section;    /* Compiled lines for code section */
    Symbol * symbol_table;           /* Symbol table for addressing */
    Symbol * entry_calls;            /* Table for entry calls */
    Symbol * extern_calls;           /* Table for external calls */
    unsigned int code_word_count;    /* Number of words in the code section */
    unsigned int data_word_count;    /* Number of words in the data section */
} Object_File;

Object_File first_move(FILE * am_file, const char * am_filename);
Object_File second_move(Object_File object_file);
int assembler(FILE * am_file, const char * am_filename);
Symbol* get_symbol(Symbol* symbol_table, const char* symbol_name);
Symbol* insert_symbol_to_table(Symbol* symbol, const char* symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int* address);
unsigned int encode_first_word(int opcode, int src_addr, int dest_addr);
void set_inst_extra_words(const Analyzed_line *analyzed_line, Compiled_Line *compiled_line, int num_operands, unsigned int *address);

#endif /* ASSEMBLER_H */ 
 

