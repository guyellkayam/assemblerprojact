#include "file_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the suffixes for file extensions */
#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define DOT_OB_SUFFIX ".ob"
#define MAX_FILE_NAME_LENGTH 256
#define BASE_ADDRESS 100  /* Assuming a base address */

/* Get entry_table and build '.ent' file with all the entries definitions' lines. */
int build_entry_file(char *entry_filename, Symbol *entry_table)
{
    FILE *entry_file;
    char full_filename[MAX_FILE_NAME_LENGTH];
    size_t filename_length;

    filename_length = strlen(entry_filename);
    if (filename_length + strlen(DOT_ENT_SUFFIX) >= MAX_FILE_NAME_LENGTH)
    {
        fprintf(stderr, "Filename too long for entry file\n");
        return 0;
    }

    strncpy(full_filename, entry_filename, MAX_FILE_NAME_LENGTH - strlen(DOT_ENT_SUFFIX) - 1);
    strcat(full_filename, DOT_ENT_SUFFIX);

    printf("Attempting to create entry file: %s\n", full_filename);

    entry_file = fopen(full_filename, "w");
    if (entry_file == NULL)
    {
        fprintf(stderr, "Failed to open entry file: %s\n", full_filename);
        return 0;
    }

    if (entry_table == NULL)
    {
        fprintf(stderr, "Entry table is empty\n");
        fclose(entry_file);
        return 0;
    }

    while (entry_table != NULL)
    {
        fprintf(entry_file, "%s %d\n", entry_table->symbol_name, entry_table->address);
        printf("Debug: Writing entry: %s %04d\n", entry_table->symbol_name, entry_table->address);
        entry_table = entry_table->next_symbol;
    }

    fclose(entry_file);
    printf("Successfully created entry file: %s\n", full_filename);
    return 1;
}

/* Get extern_table and build '.ext' file with all the addresses where extern symbols are called. */
void build_extern_file(char *extern_filename, Symbol *extern_table)
{
    FILE *extern_file;
    char full_filename[MAX_FILE_NAME_LENGTH];
    size_t filename_length;

    filename_length = strlen(extern_filename);
    if (filename_length + strlen(DOT_EXT_SUFFIX) >= MAX_FILE_NAME_LENGTH)
    {
        fprintf(stderr, "Filename too long for extern file\n");
        return;
    }

    strncpy(full_filename, extern_filename, MAX_FILE_NAME_LENGTH - strlen(DOT_EXT_SUFFIX) - 1);
    strcat(full_filename, DOT_EXT_SUFFIX);

    extern_file = fopen(full_filename, "w");
    if (extern_file == NULL)
    {
        fprintf(stderr, "Failed to open extern file: %s\n", full_filename);
        return;
    }

    while (extern_table != NULL)
    {
        fprintf(extern_file, "%s %04d\n", extern_table->symbol_name, extern_table->address);
        printf("Debug: Writing extern: %s %04d\n", extern_table->symbol_name, extern_table->address);
        extern_table = extern_table->next_symbol;
    }

    fclose(extern_file);
}

/* Get the compiled code_section and data_section and build an '.ob' file from them. */
void build_ob_file(char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section) {
    FILE *ob_file;
    unsigned int code_length = 0;
    unsigned int data_length = 0;
    unsigned int address = BASE_ADDRESS;
    int i;
    Compiled_Line *current;
    char full_filename[MAX_FILE_NAME_LENGTH];
    size_t filename_length;

    filename_length = strlen(ob_filename);
    if (filename_length + strlen(DOT_OB_SUFFIX) >= MAX_FILE_NAME_LENGTH)
    {
        fprintf(stderr, "Filename too long for object file\n");
        return;
    }

    strncpy(full_filename, ob_filename, MAX_FILE_NAME_LENGTH - strlen(DOT_OB_SUFFIX) - 1);
    strcat(full_filename, DOT_OB_SUFFIX);

    ob_file = fopen(full_filename, "w");
    if (ob_file == NULL)
    {
        fprintf(stderr, "Failed to open object file: %s\n", full_filename);
        return;
    }

    /* Calculate lengths of code and data sections */
    current = code_section;
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

    /* Write header line */
    fprintf(ob_file, "%d %d\n", code_length - 1, data_length);
    printf("Debug: Object file header - Code length: %d, Data length: %d\n", code_length, data_length);

    /* Write code section */
    current = code_section;
    while (current != NULL)
    {
        for (i = 0; i < current->num_of_words; i++)
        {
            fprintf(ob_file, "%04d %05o\n", address, current->words[i] & 0x7FFF);
            printf("Debug: Object file line: %04d %05d\n", address, current->words[i] & 0xFFFF);
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
            fprintf(ob_file, "%04d %05d\n", address, current->words[i] & 0xFFFF);
            printf("Debug: Object file line: %04d %05d\n", address, current->words[i] & 0xFFFF);


            address++;
        }
        current = current->next_compiled_line;
    }

    fclose(ob_file);
} 
