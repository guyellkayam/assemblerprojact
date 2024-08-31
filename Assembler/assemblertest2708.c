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

    while (fgets(line, sizeof(line), am_file)) 
    {
        analyzed_line = get_analyzed_line(line);

        /* Check For Syntax Error */
        if (!is_valid_analyzed_line(&analyzed_line))
        {
            assembler_error(line_index, analyzed_line.syntax_error);
        }

        /* Add main label to the symbol table */
        if (strlen(analyzed_line.label_name) > 0)
        {
            temp_symbol = get_symbol(symbol, analyzed_line.label_name);
            if (temp_symbol)
            {
                if (temp_symbol->symbol_opt == symbol_entry_def) 
                {
                    symbol = insert_symbol_to_table(symbol, analyzed_line.label_name, line_index, 
                        (analyzed_line.analyzed_line_opt == directive) ? symbol_entry_data : symbol_entry_code, 
                        &address);
                }
                else 
                {
                    assembler_error(line_index, "Redeclaration of label");
                }
            }
            else 
            {
                symbol = insert_symbol_to_table(symbol, analyzed_line.label_name, line_index, 
                    (analyzed_line.analyzed_line_opt == directive) ? symbol_local_data : symbol_local_code, 
                    &address);
            }
        }

        /* Compile (First Move) Directive */
        if (analyzed_line.analyzed_line_opt == directive)
        {
            /* Add directive to the data section */
            data_section = insert_compiled_line_to_table(data_section, line_index, &address);
            current_compiled_line = get_compiled_line(data_section, line_index);

            /* Compile String Directive */
            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_string)
            {
                for (i = 0; analyzed_line.dir_or_inst.directive.dir_operand.string[i] != '\0'; i++)
                {
                    insert_word(current_compiled_line, analyzed_line.dir_or_inst.directive.dir_operand.string[i], &address);
                }
                insert_word(current_compiled_line, '\0', &address);
                line_index++;
                continue;
            }

            /* Compile Data Directive */
            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_data) 
            {
                for (i = 0; i < analyzed_line.dir_or_inst.directive.dir_operand.data.data_count; i++)
                {
                    insert_word(current_compiled_line, analyzed_line.dir_or_inst.directive.dir_operand.data.data[i], &address);
                }
                line_index++;
                continue;
            }

            /* Entry and Extern Definitions */
            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry || analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
            {
                temp_symbol = get_symbol(symbol, analyzed_line.dir_or_inst.directive.dir_operand.label_name);
                if (temp_symbol)
                {
                    if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry)
                    {
                        if (temp_symbol->symbol_opt == symbol_local_data || temp_symbol->symbol_opt == symbol_local_code)
                        {
                            temp_symbol->symbol_opt = (temp_symbol->symbol_opt == symbol_local_data) ? symbol_entry_data : symbol_entry_code;
                        }
                        else if (temp_symbol->symbol_opt == symbol_extern_def)
                        {
                            assembler_error(line_index, "Entry label is defined as extern");
                        }
                    }
                    else if (analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
                    {
                        if (temp_symbol->symbol_opt != symbol_extern_def)
                        {
                            assembler_error(line_index, "Extern label already defined as non-extern");
                        }
                    }
                }
                else
                {
                    unsigned int temp_address = 0;  /* External symbols don't have a real address */
                    symbol = insert_symbol_to_table(symbol, analyzed_line.dir_or_inst.directive.dir_operand.label_name, 
                        line_index, (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry) ? symbol_entry_def : symbol_extern_def, 
                        &temp_address);
                }
                line_index++;
                continue;
            }
        }

        /* Compile (First Move) Instruction */
        if (analyzed_line.analyzed_line_opt == instruction) 
        {
            /* Add Instruction to the code section */
            code_section = insert_compiled_line_to_table(code_section, line_index, &address);
            
            /* Insert the inst word to the current compiled_line in the code_section */
            inst_word = analyzed_line.dir_or_inst.instruction.inst_opt << OPCODE_INDENTATION;
            
            /* Handle Operands */
            if (analyzed_line.dir_or_inst.instruction.inst_operand_options[0] == operand_none && 
                analyzed_line.dir_or_inst.instruction.inst_operand_options[1] == operand_none)
            {
                current_compiled_line = get_compiled_line(code_section, line_index);
                insert_word(current_compiled_line, inst_word, &address);
            } 
            else if (analyzed_line.dir_or_inst.instruction.inst_operand_options[0] == operand_none || 
                     analyzed_line.dir_or_inst.instruction.inst_operand_options[1] == operand_none)
            {
                dest_operand_i = 0;
                num_of_operands = 1;
                inst_word |= analyzed_line.dir_or_inst.instruction.inst_operand_options[dest_operand_i] << DEST_INDENTATION;
                current_compiled_line = get_compiled_line(code_section, line_index);
                insert_word(current_compiled_line, inst_word, &address);
            }
            else 
            {
                dest_operand_i = 1;
                src_operand_i = 0;
                num_of_operands = 2;
                inst_word |= analyzed_line.dir_or_inst.instruction.inst_operand_options[dest_operand_i] << DEST_INDENTATION;
                inst_word |= analyzed_line.dir_or_inst.instruction.inst_operand_options[src_operand_i] << SRC_INDENTATION;
                current_compiled_line = get_compiled_line(code_section, line_index);
                insert_word(current_compiled_line, inst_word, &address);
            }     

            /* Add Extra Words */
            set_inst_extra_words(&analyzed_line, current_compiled_line, num_of_operands, &address);
            line_index++;
        }
    } 

    entry_calls = get_entry_calls(symbol, entry_calls);
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

    while (current_section != NULL) 
    {
        unsigned int current_address = 0;
        Compiled_Line *original_current_line = NULL;

        /* Index '0' of words is the instruction word. That's why 'num_of_words - 1'. */
        for (i = 0; i < current_section->num_of_words - 1; i++)
        {
            if (strlen(current_section->missing_label[i]) > 0)
            {
                original_current_line = get_compiled_line(current_section, current_section->line_index);

                /* Build extern_calls table */
                temp_symbol = get_symbol(object_file.symbol_table, current_section->missing_label[i]);
                
                if (temp_symbol == NULL) 
                {
                    printf("Warning: Symbol '%s' not found in symbol table. Assuming external.\n", 
                           current_section->missing_label[i]);
                    extern_calls = insert_symbol_to_table(extern_calls, 
                                                          current_section->missing_label[i], 
                                                          current_section->line_index, 
                                                          symbol_extern_def, 
                                                          &(current_section->begin_address) + 1);
                    current_address = 0 | ARE_EXTERN;
                }
                else if (temp_symbol->symbol_opt == symbol_extern_def)
                {
                    extern_calls = insert_symbol_to_table(extern_calls, 
                                                          current_section->missing_label[i], 
                                                          current_section->line_index, 
                                                          symbol_extern_def, 
                                                          &(current_section->begin_address) + 1);
                    current_address = 0 | ARE_EXTERN;
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

    object_file = first_move(am_file, am_filename);
    object_file = second_move(object_file); /* Fix the symbols' addresses and build extern_calls table. */

    if (object_file.entry_calls != NULL)
    {
        remove_suffix(am_filename, entry_filename, DOT_AM_SUFFIX);
        build_entry_file(entry_filename, object_file.entry_calls);
    }

    if (object_file.extern_calls != NULL)
    {
        remove_suffix(am_filename, extern_filename, DOT_AM_SUFFIX);
        build_extern_file(extern_filename, object_file.extern_calls);
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
