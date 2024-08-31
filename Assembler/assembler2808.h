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
/* #define BASE_ADDRESS 0 */


typedef struct Object_File 
{
    Compiled_Line * data_section;    /**< Compiled lines for data section */
    Compiled_Line * code_section;    /**< Compiled lines for code section */
    Symbol * symbol_table;           /**< Symbol table for addressing */
    Symbol * entry_calls;            /**< Table for entry calls */
    Symbol * extern_calls;           /**< Table for external calls */
} Object_File;

/**
 * @brief First pass over the .am file to create initial data structures.
 * 
 * @param am_file Pointer to the .am file.
 * @param am_filename The .am filename.
 *
 * @return Struct that contains: Symbol table, code_section, data_section, entry_calls.
 */
Object_File first_move(FILE * am_file, const char * am_filename);

/**
 * @brief Second pass over the code_section to update symbol addresses and build extern_calls table.
 *
 * @param object_file Pointer to the Object_File struct.
 * @return Updated Object_File struct with required information for creating .ob, .ent, .ext files.
 */
Object_File second_move(Object_File object_file);

/**
 * @brief Executes the assembler process, including both passes and file creation.
 * 
 * @param am_file Pointer to the .am file.
 * @param am_filename The .am filename.
 * @return 1 if successful, 0 if failed.
 */
int assembler(FILE * am_file, const char * am_filename);

#endif /* ASSEMBLER_H */ 

