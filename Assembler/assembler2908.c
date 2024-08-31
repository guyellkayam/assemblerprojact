#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../helpers/assembler_helper.h"
#include "../file_builder/file_builder.h"
#include "../data_structures/symbol_table.h"

/* First move on the am_file. */
Object_File first_move(FILE *am_file, const char *am_filename)
{
    int i;
    char line[MAX_LINE_LENGTH] = {0}; /* Initialize line */
    Analyzed_line analyzed_line;
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
    /* Symbol *existing_symbol = NULL; */

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
                    temp_symbol->address = address;
                    temp_symbol->symbol_opt = (analyzed_line.analyzed_line_opt == directive) ? symbol_entry_data : symbol_entry_code;
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
            printf("Debug: Label %s set with address %u\n", analyzed_line.label_name, address);
        }

        /* Compile (First Move) Directive */
        if (analyzed_line.analyzed_line_opt == directive)
{
        /* Entry and Extern Definitions */
        if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry || analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
        {
            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry)
        {
            Symbol *existing_symbol;
            printf("Debug: Found .entry directive for symbol %s\n", analyzed_line.dir_or_inst.directive.dir_operand.string);
            existing_symbol = get_symbol(symbol, analyzed_line.dir_or_inst.directive.dir_operand.string);
            if (existing_symbol != NULL) {
                existing_symbol->symbol_opt = symbol_entry_def;
                printf("Debug: Updated existing symbol %s to entry\n", existing_symbol->symbol_name);
            } else {
                unsigned int current_address = address;  // Use the current address
                symbol = insert_symbol_to_table(symbol, analyzed_line.dir_or_inst.directive.dir_operand.string, line_index, symbol_entry_def, &current_address);
            }
        }
            else if (analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
            {
                printf("Debug: Found .extern directive for symbol %s\n", analyzed_line.dir_or_inst.directive.dir_operand.string);
                symbol = insert_symbol_to_table(symbol, analyzed_line.dir_or_inst.directive.dir_operand.string, line_index, symbol_extern_def, NULL);
            }
            line_index++;
            continue;
        }

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
        }

        /* Compile (First Move) Instruction */
        if (analyzed_line.analyzed_line_opt == instruction) 
        {
            /* Add Instruction to the code section */
            code_section = insert_compiled_line_to_table(code_section, line_index, &address);
            current_compiled_line = get_compiled_line(code_section, line_index);

            /* Encode first word (opcode and addressing modes) */
            inst_word = (analyzed_line.dir_or_inst.instruction.inst_opt << 12) & 0xF000;
            inst_word |= (analyzed_line.dir_or_inst.instruction.inst_operand_options[0] << 9) & 0x0E00;
            inst_word |= (analyzed_line.dir_or_inst.instruction.inst_operand_options[1] << 2) & 0x001C;
            insert_word(current_compiled_line, inst_word, &address);

            /* Encode additional words for operands */
            for (i = 0; i < 2; i++) {
                switch (analyzed_line.dir_or_inst.instruction.inst_operand_options[i]) {
                    case operand_none:
                        /* No additional word needed for operand_none */
                        break;
                    case operand_const_number:
                        insert_word(current_compiled_line, analyzed_line.dir_or_inst.instruction.inst_operands[i].const_number, &address);
                        break;
                    case operand_label:
                    case operand_label_indirect:
                        /* Insert a placeholder, to be resolved in second pass */
                        insert_word(current_compiled_line, 0, &address);
                        strcpy(current_compiled_line->missing_label[current_compiled_line->num_of_words - 1], 
                               analyzed_line.dir_or_inst.instruction.inst_operands[i].label);
                        break;
                    case operand_register:
                    case operand_register_indirect:
                        /* If both operands are registers, encode them in one word */
                        if (i == 0 && analyzed_line.dir_or_inst.instruction.inst_operand_options[1] == operand_register) {
                            unsigned int reg_word = (analyzed_line.dir_or_inst.instruction.inst_operands[0].register_number << 6) & 0x01C0;
                            reg_word |= analyzed_line.dir_or_inst.instruction.inst_operands[1].register_number & 0x0007;
                            insert_word(current_compiled_line, reg_word, &address);
                            i++; /* Skip next iteration */
                        } else {
                            insert_word(current_compiled_line, analyzed_line.dir_or_inst.instruction.inst_operands[i].register_number, &address);
                        }
                        break;
                }
            }

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
    Compiled_Line *current_section = object_file.code_section;
    Symbol *entry;
    Symbol *symbol;
    unsigned int symbol_address;
    unsigned int extern_address;

    while (current_section != NULL) 
    {
        for (i = 0; i < current_section->num_of_words; i++)
        {
            if (strlen(current_section->missing_label[i]) > 0)
            {
                symbol = get_symbol(object_file.symbol_table, current_section->missing_label[i]);
                if (symbol)
                {
                    symbol_address = symbol->address;
                    if (symbol->symbol_opt == symbol_extern_def)
                    {
                        symbol_address = 1; /* External symbol */
                        extern_address = current_section->begin_address + i;
                        extern_calls = insert_symbol_to_table(extern_calls, 
                                                              current_section->missing_label[i], 
                                                              current_section->line_index, 
                                                              symbol_extern_def, 
                                                              &extern_address);
                        printf("Debug: External symbol %s used at address %u\n", current_section->missing_label[i], extern_address);
                    }
                    else
                    {
                        symbol_address = (symbol_address << 2) | 2; /* Relocatable address */
                    }
                    current_section->words[i] = symbol_address;
                }
                else
                {
                    /* Handle error: undefined symbol */
                    assembler_error(current_section->line_index, "Undefined symbol");
                }
            }
        }
        current_section = current_section->next_compiled_line;
    }

    /* Update entry symbol addresses */
    entry = object_file.entry_calls;
    while (entry != NULL)
    {
        symbol = get_symbol(object_file.symbol_table, entry->symbol_name);
        if (symbol != NULL)
        {
            entry->address = symbol->address;
            printf("Debug: Updated entry symbol %s with address %u\n", entry->symbol_name, entry->address);
        }
        entry = entry->next_symbol;
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
        printf("Debug: Creating .ent file\n");
        remove_suffix(am_filename, entry_filename, DOT_AM_SUFFIX);
        build_entry_file(entry_filename, object_file.entry_calls);
        printf("Debug: .ent file creation attempt completed\n");
    }
    else
    {
        printf("Debug: No entry symbols found\n");
    }

    if (object_file.extern_calls != NULL)
    {
        printf("Debug: Creating .ext file\n");
        remove_suffix(am_filename, extern_filename, DOT_AM_SUFFIX);
        build_extern_file(extern_filename, object_file.extern_calls);
        printf("Debug: .ext file creation attempt completed\n");
    }
    else
    {
        printf("Debug: No external symbols used\n");
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

