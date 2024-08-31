#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../helpers/assembler_helper.h"
#include "../file_builder/file_builder.h"

/* First move on the am_file. */
Object_File first_move(FILE *am_file, const char *am_filename)
{
    int i;
    char line[MAX_LINE_LENGTH] = {0}; /* Initialize line */
    Analyzed_line analyzed_line;
    int dest_operand_i;
    int src_operand_i;
    int num_of_operands;
    Compiled_Line *data_section = NULL;
    Compiled_Line *code_section = NULL;
    Compiled_Line *current_compiled_line = NULL;
    Symbol *symbol = NULL;
    Symbol *temp_symbol = NULL;
    Symbol *entry_calls = NULL;
    Object_File object_file;
    unsigned int address = BASE_ADDRESS;
    int line_index = 1;
    unsigned int inst_word = 0;
    Symbol *debug_entry = NULL; /* For debugging entry symbols */

    while (fgets(line, sizeof(line), am_file)) 
    {
        /* ... (rest of the function remains the same) ... */
    } 

    entry_calls = get_entry_calls(symbol, entry_calls);
    printf("Number of entry symbols found: %d\n", count_symbols(entry_calls));
    
    /* Debug print for entry symbols */
    debug_entry = entry_calls;
    while (debug_entry != NULL) {
        printf("Debug: Entry symbol %s, Address: %d\n", debug_entry->symbol_name, debug_entry->address);
        debug_entry = debug_entry->next_symbol;
    }
    
    object_file.code_section = code_section;
    object_file.data_section = data_section;
    object_file.symbol_table = symbol;
    object_file.entry_calls = entry_calls;

    fclose(am_file);
    return object_file;
}

/* Second move goes over the code_section to update symbols' addresses and build extern_calls table. */
Object_File second_move(Object_File object_file)
{
    int i;
    Symbol *extern_calls = NULL;
    Symbol *temp_symbol = NULL;
    Compiled_Line *current_section = object_file.code_section;
    unsigned int extern_address;

    while (current_section != NULL) 
    {
        unsigned int current_address = 0;
        Compiled_Line *original_current_line = NULL;

        for (i = 0; i < current_section->num_of_words; i++)
        {
            if (strlen(current_section->missing_label[i]) > 0)
            {
                original_current_line = get_compiled_line(current_section, current_section->line_index);

                /* Build extern_calls table */
                temp_symbol = get_symbol(object_file.symbol_table, current_section->missing_label[i]);
                
                if (temp_symbol == NULL || temp_symbol->symbol_opt == symbol_extern_def)
                {
                    extern_address = current_section->begin_address + i;
                    extern_calls = insert_symbol_to_table(extern_calls, 
                                                          current_section->missing_label[i], 
                                                          current_section->line_index, 
                                                          symbol_extern_def, 
                                                          &extern_address);
                    current_address = 0 | ARE_EXTERN;
                    printf("Debug: External symbol %s at address %d\n", current_section->missing_label[i], extern_address);
                }
                else
                {
                    current_address = get_symbol_def_address(object_file.symbol_table, current_section->missing_label[i]); 
                }

                if (current_address == 0) 
                {
                    assembler_error(current_section->line_index, "Can't find the address of the label in the Symbol Table");
                }
                original_current_line->words[i + 1] = current_address;
            }
        }

        current_section = current_section->next_compiled_line;
    }

    /* Save the extern_calls table */
    object_file.extern_calls = extern_calls;
    return object_file;
}

/* Calls to the first and the second moves and the relevant methods to create the following files: '.ob' .ent' '.ext'. */
int assembler(FILE *am_file, const char *am_filename)
{
    Object_File object_file;
    char entry_filename[MAX_FILE_NAME_LENGTH] = {0};
    char extern_filename[MAX_FILE_NAME_LENGTH] = {0};
    char ob_filename[MAX_FILE_NAME_LENGTH] = {0};
    Symbol *current;

    object_file = first_move(am_file, am_filename);
    object_file = second_move(object_file); /* Fix the symbols' addresses and build extern_calls table. */

    printf("Entry calls address: %p\n", (void*)object_file.entry_calls);
    if (object_file.entry_calls != NULL)
    {
        printf("First entry symbol: %s\n", object_file.entry_calls->symbol_name);
        current = object_file.entry_calls;
        while (current != NULL)
        {
            printf("Entry symbol: %s, Address: %d\n", current->symbol_name, current->address);
            current = current->next_symbol;
        }
        remove_suffix(am_filename, entry_filename, DOT_AM_SUFFIX);
        if (!build_entry_file(entry_filename, object_file.entry_calls))
        {
            fprintf(stderr, "Failed to build entry file\n");
        }
    }
    else
    {
        printf("No entry symbols found\n");
    }

    /* Debug print for extern calls */
    if (object_file.extern_calls != NULL)
    {
        printf("External symbols:\n");
        current = object_file.extern_calls;
        while (current != NULL)
        {
            printf("External symbol: %s, Address: %d\n", current->symbol_name, current->address);
            current = current->next_symbol;
        }
        remove_suffix(am_filename, extern_filename, DOT_AM_SUFFIX);
        build_extern_file(extern_filename, object_file.extern_calls);
    }
    else
    {
        printf("No external symbols found\n");
    }
    
    remove_suffix(am_filename, ob_filename, DOT_OB_SUFFIX);
    build_ob_file(ob_filename, object_file.code_section, object_file.data_section);

    /* Free allocated memory */
    free(object_file.extern_calls);
    free(object_file.entry_calls);
    free_symbol_table(object_file.symbol_table);
    free_compiled_line_table(object_file.data_section);
    free_compiled_line_table(object_file.code_section);

    return TRUE;
}