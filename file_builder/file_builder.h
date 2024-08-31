#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H

#include "../assembler/assembler.h"

#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define DOT_OB_SUFFIX ".ob"
#define MAX_FILE_NAME_LENGTH 256

/**
 * @brief Get entry_table and build '.ent' file with all the entries definitions' lines.
 * 
 * @param entry_filename The base filename without the '.am' suffix.
 * @param entry_table Table with all the entries definitions' lines.
 */
int build_entry_file(char *entry_filename, Symbol *entry_table);

/**
 * @brief Get extern_table and build '.ext' file with all the addresses where extern symbols are called.
 * 
 * @param extern_filename The base filename without the '.am' suffix.
 * @param extern_table Table with all the addresses where extern symbols are called.
 */
void build_extern_file(char *extern_filename, Symbol *extern_table);

/**
 * @brief Get the compiled code_section and data_section and build an '.ob' file from them.
 * 
 * @param ob_filename The base filename without the '.am' suffix.
 * @param code_section The compiled code section.
 * @param data_section The compiled data section.
 * @param code_word_count The number of words in the code section.
 * @param data_word_count The number of words in the data section.
 */
void build_ob_file(char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section, unsigned int code_word_count, unsigned int data_word_count);

#endif /* FILE_BUILDER_H */ 
