#include "file_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int build_entry_file(char *entry_filename, Symbol *entry_table)
{
    FILE *entry_file;
    char full_filename[MAX_FILE_NAME_LENGTH];

    snprintf(full_filename, sizeof(full_filename), "%s%s", entry_filename, DOT_ENT_SUFFIX);

    entry_file = fopen(full_filename, "w");
    if (entry_file == NULL)
    {
        fprintf(stderr, "Failed to open entry file: %s\n", full_filename);
        return 0;
    }

    while (entry_table != NULL)
    {
        if (entry_table->symbol_opt == symbol_entry_def && entry_table->address != 0)
        {
            fprintf(entry_file, "%s %d\n", entry_table->symbol_name, entry_table->address);
            printf("Debug: Writing entry: %s %d\n", entry_table->symbol_name, entry_table->address);
        }
        entry_table = entry_table->next_symbol;
    }

    fclose(entry_file);
    printf("Successfully created entry file: %s\n", full_filename);
    return 1;
}

void build_extern_file(char *extern_filename, Symbol *extern_table)
{
    FILE *extern_file;
    char full_filename[MAX_FILE_NAME_LENGTH];
    Symbol *current;

    snprintf(full_filename, sizeof(full_filename), "%s%s", extern_filename, DOT_EXT_SUFFIX);

    extern_file = fopen(full_filename, "w");
    if (extern_file == NULL)
    {
        fprintf(stderr, "Failed to open extern file: %s\n", full_filename);
        return;
    }

    current = extern_table;
    while (current != NULL)
    {
        /* Write all addresses for the current symbol */
        do
        {
            fprintf(extern_file, "%s %d\n", current->symbol_name, current->address);
            printf("Debug: Writing extern: %s %d\n", current->symbol_name, current->address);
            current = current->next_symbol;
        } while (current != NULL && strcmp(current->symbol_name, extern_table->symbol_name) == 0);

        extern_table = current;
    }

    fclose(extern_file);
    printf("Successfully created extern file: %s\n", full_filename);
}

void build_ob_file(char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section, 
                   unsigned int code_word_count, unsigned int data_word_count)
{
    FILE *ob_file;
    unsigned int address = BASE_ADDRESS;
    int i;
    Compiled_Line *current;
    char full_filename[MAX_FILE_NAME_LENGTH];

    snprintf(full_filename, sizeof(full_filename), "%s%s", ob_filename, DOT_OB_SUFFIX);

    ob_file = fopen(full_filename, "w");
    if (ob_file == NULL)
    {
        fprintf(stderr, "Failed to open object file: %s\n", full_filename);
        return;
    }

    /* Write header line */
    fprintf(ob_file, "%d %d\n", code_word_count, data_word_count);
    printf("Debug: Object file header - Code words: %d, Data words: %d\n", code_word_count, data_word_count);

    /* Write code section */
    current = code_section;
    while (current != NULL)
    {
        for (i = 0; i < current->num_of_words; i++)
        {
            fprintf(ob_file, "%04d %05o\n", address, current->words[i] & 0x7FFF);
            printf("Debug: Code word at address %04d: %05o\n", address, current->words[i] & 0x7FFF);
            address++;
        }
        current = current->next_compiled_line;
    }

    /* Write data section */
    current = data_section;
    while (current != NULL)
    {
        for (i = 0; i < current->num_of_words; i++)
        {
            fprintf(ob_file, "%04d %05o\n", address, current->words[i] & 0x7FFF);
            printf("Debug: Data word at address %04d: %05o\n", address, current->words[i] & 0x7FFF);
            address++;
        }
        current = current->next_compiled_line;
    }

    fclose(ob_file);
    printf("Debug: Object file created: %s\n", full_filename);
} 
