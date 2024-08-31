/* file_builder.h */

#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H

#include "../data_structures/symbol_table.h"
#include "../data_structures/compiled_line.h"

int build_entry_file(const char *entry_filename, Symbol *entry_table);
int build_extern_file(const char *extern_filename, Symbol *extern_table);
int build_ob_file(const char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section);
unsigned int count_words(Compiled_Line *section);

#endif /* FILE_BUILDER_H */

/* file_builder.c */

#include "file_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define DOT_OB_SUFFIX ".ob"
#define MAX_FILE_NAME_LENGTH 256
#define BASE_ADDRESS 100

static void write_section(FILE *file, Compiled_Line *section, unsigned int *address);

int build_entry_file(const char *entry_filename, Symbol *entry_table)
{
    FILE *entry_file;
    char full_filename[MAX_FILE_NAME_LENGTH];

    if (!entry_filename || !entry_table) {
        fprintf(stderr, "Invalid input for entry file creation\n");
        return 0;
    }

    snprintf(full_filename, sizeof(full_filename), "%s%s", entry_filename, DOT_ENT_SUFFIX);

    entry_file = fopen(full_filename, "w");
    if (!entry_file) {
        perror("Failed to open entry file");
        return 0;
    }

    while (entry_table) {
        fprintf(entry_file, "%s\t%u\n", entry_table->symbol_name, entry_table->address);
        entry_table = entry_table->next_symbol;
    }

    fclose(entry_file);
    printf("Successfully created entry file: %s\n", full_filename);
    return 1;
}

int build_extern_file(const char *extern_filename, Symbol *extern_table)
{
    FILE *extern_file;
    char full_filename[MAX_FILE_NAME_LENGTH];

    if (!extern_filename || !extern_table) {
        fprintf(stderr, "Invalid input for extern file creation\n");
        return 0;
    }

    snprintf(full_filename, sizeof(full_filename), "%s%s", extern_filename, DOT_EXT_SUFFIX);

    extern_file = fopen(full_filename, "w");
    if (!extern_file) {
        perror("Failed to open extern file");
        return 0;
    }

    while (extern_table) {
        fprintf(extern_file, "%s\t%04u\n", extern_table->symbol_name, extern_table->address);
        extern_table = extern_table->next_symbol;
    }

    fclose(extern_file);
    printf("Successfully created extern file: %s\n", full_filename);
    return 1;
}

int build_ob_file(const char *ob_filename, Compiled_Line *code_section, Compiled_Line *data_section)
{
    FILE *ob_file;
    char full_filename[MAX_FILE_NAME_LENGTH];
    unsigned int address = BASE_ADDRESS;
    unsigned int code_length = 0, data_length = 0;

    if (!ob_filename || (!code_section && !data_section)) {
        fprintf(stderr, "Invalid input for object file creation\n");
        return 0;
    }

    snprintf(full_filename, sizeof(full_filename), "%s%s", ob_filename, DOT_OB_SUFFIX);

    ob_file = fopen(full_filename, "w");
    if (!ob_file) {
        perror("Failed to open object file");
        return 0;
    }

    /* Calculate lengths */
    code_length = count_words(code_section);
    data_length = count_words(data_section);

    /* Write header */
    fprintf(ob_file, "%u %u\n", code_length, data_length);

    /* Write sections */
    write_section(ob_file, code_section, &address);
    write_section(ob_file, data_section, &address);

    fclose(ob_file);
    printf("Successfully created object file: %s\n", full_filename);
    return 1;
}

static void write_section(FILE *file, Compiled_Line *section, unsigned int *address)
{
    int i;
    while (section) {
        for (i = 0; i < section->num_of_words; i++) {
            fprintf(file, "%04u %05o\n", *address, section->words[i] & 0xFFFF);
            (*address)++;
        }
        section = section->next_compiled_line;
    }
}

unsigned int count_words(Compiled_Line *section)
{
    unsigned int count = 0;
    while (section) {
        count += section->num_of_words;
        section = section->next_compiled_line;
    }
    return count;
} 
