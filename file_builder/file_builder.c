#include "file_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* @brief Get entry_table and build '.ent' file with all the entries definitions' lines. */
void build_entry_file(char *entry_filename, Symbol *entry_table)
{
    FILE *entry_file;

    strcat(entry_filename, DOT_ENT_SUFFIX);
    entry_file = fopen(entry_filename, "w");

    if (entry_file == NULL)
    {
        printf("Failed to open entry_file\n");
        return;
    }

    while (entry_table != NULL)
    {
        fprintf(entry_file, "%s\t%d\n", entry_table->symbol_name, entry_table->address);
        entry_table = entry_table->next_symbol;
    }

    fclose(entry_file);
}

/* Get extern_table and build '.ext' file with all the addresses where extern symbols are called. */
void build_extern_file(char *extern_filename, Symbol *extern_table)
{
    FILE *extern_file;

    strcat(extern_filename, DOT_EXT_SUFFIX);
    extern_file = fopen(extern_filename, "w");

    if (extern_file == NULL)
    {
        printf("Failed to open extern_file\n");
        return;
    }

    while (extern_table != NULL)
    {
        fprintf(extern_file, "%s\t%d\n", extern_table->symbol_name, extern_table->address);
        extern_table = extern_table->next_symbol;
    }

    fclose(extern_file);
}

/* Get the compiled code_section and data_section and build an '.ob' file from them. */
void build_ob_file(char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section)
{
    FILE *ob_file;
    int i;
    unsigned int address = 0;

    strcat(ob_filename, DOT_OB_SUFFIX);
    ob_file = fopen(ob_filename, "w");

    if (ob_file == NULL)
    {
        printf("Failed to open ob_file\n");
        return;
    }

    // Calculate lengths of code and data sections
    unsigned int code_length = 0;
    unsigned int data_length = 0;

    Compiled_Line *current = code_section;
    while (current != NULL)
    {
        code_length += current->num_of_words;
        current = current->next_compiled_line;
    }

    current = data_section;
    while (current != NULL)
    {
        data_length += current->num_of_words;
        current = current->next_compiled_line;
    }

    // Write header line
    fprintf(ob_file, "%04d %04d\n", code_length, data_length);

    // Write code section
    current = code_section;
    while (current != NULL)
    {
        for (i = 0; i < current->num_of_words; i++)
        {
            fprintf(ob_file, "%04d %05o\n", address, current->words[i]);
            address++;
        }
        current = current->next_compiled_line;
    }

    // Write data section
    while (data_section != NULL)
    {
        for (i = 0; i < data_section->num_of_words; i++)
        {
            fprintf(ob_file, "%04d %05o\n", address, data_section->words[i]);
            address++;
        }
        data_section = data_section->next_compiled_line;
    }

    fclose(ob_file);
}
