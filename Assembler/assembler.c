#include "assembler.h"
#include "../helpers/assembler_helper.h"
#include "../file_builder/file_builder.h"
#include <stdlib.h>
#include <string.h>

unsigned int encode_first_word(int opcode, int src_addr, int dest_addr) {
    unsigned int word = 0;
    word |= (opcode & 0x3F) << 6;
    word |= (src_addr & 0x3) << 4;
    word |= (dest_addr & 0x3) << 2;
    /* ARE bits are set to 00 by default */
    printf("Debug: Encoding first word - opcode: %d, src_addr: %d, dest_addr: %d\n", opcode, src_addr, dest_addr);
    printf("Debug: Encoded first word: %05o\n", word);
    return word;
}

/* Encoding for register operands */
unsigned int encode_register(int reg_num) {
    return (reg_num & 0x7) << 2;
}

/* Encoding for immediate values */
unsigned int encode_immediate(int value) {
    return (value & 0xFFFF) << 2;  /* Allow for 16-bit values */
}

/* Encoding for addressing modes */
unsigned int encode_addressing_mode(int mode, int value) {
    unsigned int encoded = mode << 2;
    encoded |= value & 0x3;
    return encoded;
}

/* Specific instruction encodings */
unsigned int encode_add(int src, int dest) {
    return encode_first_word(2, src, dest);
}

unsigned int encode_sub(int src, int dest) {
    return encode_first_word(3, src, dest);
}

unsigned int encode_mov(int src, int dest) {
    return encode_first_word(0, src, dest);
}

unsigned int encode_cmp(int src, int dest) {
    return encode_first_word(1, src, dest);
}

unsigned int encode_lea(int src, int dest) {
    return encode_first_word(4, src, dest);
}

unsigned int encode_clr(int dest) {
    return encode_first_word(6, 0, dest);
}

unsigned int encode_not(int dest) {
    return encode_first_word(5, 0, dest);
}

unsigned int encode_inc(int dest) {
    return encode_first_word(7, 0, dest);
}

unsigned int encode_dec(int dest) {
    return encode_first_word(8, 0, dest);
}

unsigned int encode_jmp(int dest) {
    return encode_first_word(9, 0, dest);
}

unsigned int encode_bne(int dest) {
    return encode_first_word(10, 0, dest);
}

unsigned int encode_jsr(int dest) {
    return encode_first_word(13, 0, dest);
}

unsigned int encode_red(int dest) {
    return encode_first_word(11, 0, dest);
}

unsigned int encode_prn(int dest) {
    return encode_first_word(12, 0, dest);
}

unsigned int encode_rts(void) {
    return encode_first_word(14, 0, 0);
}

unsigned int encode_stop(void) {
    return encode_first_word(15, 0, 0);
}

Object_File first_move(FILE *am_file, const char *am_filename)
{
    int i;
    char line[MAX_LINE_LENGTH] = {0};
    Analyzed_line analyzed_line;
    Compiled_Line *data_section = NULL;
    Compiled_Line *code_section = NULL;
    Compiled_Line *current_compiled_line = NULL;
    Symbol *symbol = NULL;
    Symbol *temp_symbol = NULL;
    Symbol *entry_symbols = NULL;
    Object_File object_file = {0};  /* Initialize all fields to 0 or NULL */
    unsigned int address = BASE_ADDRESS;
    int line_index = 1;
    unsigned int inst_word = 0;
    unsigned int code_word_count = 0;
    unsigned int data_word_count = 0;

    while (fgets(line, sizeof(line), am_file)) 
    {
        analyzed_line = get_analyzed_line(line);

        if (!is_valid_analyzed_line(&analyzed_line))
        {
            assembler_error(line_index, analyzed_line.syntax_error);
        }

        if (strlen(analyzed_line.label_name) > 0)
        {
            /* Handle label definition */
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

        if (analyzed_line.analyzed_line_opt == directive)
        {
            /* Handle directives */
            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry || analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
            {
                if (analyzed_line.dir_or_inst.directive.dir_opt == dir_entry) {
                    entry_symbols = insert_symbol_to_table(entry_symbols, 
                        analyzed_line.dir_or_inst.directive.dir_operand.string, 
                        line_index, 
                        symbol_entry_def, 
                        NULL);
                }
                else if (analyzed_line.dir_or_inst.directive.dir_opt == dir_extern)
                {
                    printf("Debug: Found .extern directive for symbol %s\n", analyzed_line.dir_or_inst.directive.dir_operand.string);
                    symbol = insert_symbol_to_table(symbol, analyzed_line.dir_or_inst.directive.dir_operand.string, line_index, symbol_extern_def, NULL);
                }
                line_index++;
                continue;
            }

            data_section = insert_compiled_line_to_table(data_section, line_index, &address);
            current_compiled_line = get_compiled_line(data_section, line_index);

            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_string)
            {
                for (i = 0; analyzed_line.dir_or_inst.directive.dir_operand.string[i] != '\0'; i++)
                {
                    insert_word(current_compiled_line, (analyzed_line.dir_or_inst.directive.dir_operand.string[i] << 2) | 4, &address);
                    data_word_count++;
                }
                insert_word(current_compiled_line, 4, &address);
                data_word_count++;
                line_index++;
                continue;
            }

            if (analyzed_line.dir_or_inst.directive.dir_opt == dir_data) 
            {
                for (i = 0; i < analyzed_line.dir_or_inst.directive.dir_operand.data.data_count; i++)
                {
                    insert_word(current_compiled_line, (analyzed_line.dir_or_inst.directive.dir_operand.data.data[i] << 2) | 4, &address);
                    data_word_count++;
                }
                line_index++;
                continue;
            }
        }

        if (analyzed_line.analyzed_line_opt == instruction) 
        {
            code_section = insert_compiled_line_to_table(code_section, line_index, &address);
            current_compiled_line = get_compiled_line(code_section, line_index);

            code_word_count++;  /* First word */

            switch (analyzed_line.dir_or_inst.instruction.inst_opt) {
                case inst_mov:
                case inst_cmp:
                case inst_add:
                case inst_sub:
                case inst_lea:
                    inst_word = encode_first_word(analyzed_line.dir_or_inst.instruction.inst_opt,
                                                  analyzed_line.dir_or_inst.instruction.inst_operand_options[0],
                                                  analyzed_line.dir_or_inst.instruction.inst_operand_options[1]);
                    insert_word(current_compiled_line, inst_word, &address);
                    set_inst_extra_words(&analyzed_line, current_compiled_line, 2, &address, &code_word_count);
                    break;
                case inst_not:
                case inst_clr:
                case inst_inc:
                case inst_dec:
                case inst_jmp:
                case inst_bne:
                case inst_red:
                case inst_prn:
                case inst_jsr:
                    inst_word = encode_first_word(analyzed_line.dir_or_inst.instruction.inst_opt, 0,
                                                  analyzed_line.dir_or_inst.instruction.inst_operand_options[0]);
                    insert_word(current_compiled_line, inst_word, &address);
                    set_inst_extra_words(&analyzed_line, current_compiled_line, 1, &address, &code_word_count);
                    break;
                case inst_rts:
                case inst_stop:
                    inst_word = encode_first_word(analyzed_line.dir_or_inst.instruction.inst_opt, 0, 0);
                    insert_word(current_compiled_line, inst_word, &address);
                    break;
                default:
                    assembler_error(line_index, "Unknown instruction type");
                    break;
            }

            printf("Debug: Encoding instruction %s\n", get_inst_name(analyzed_line.dir_or_inst.instruction.inst_opt));
            printf("Debug: First word: %05o\n", inst_word);

            line_index++;
        }
        printf("Debug: Processed line %d, current address: %u\n", line_index, address);
    } 

    object_file.code_section = code_section;
    object_file.data_section = data_section;
    object_file.symbol_table = symbol;
    object_file.entry_symbols = entry_symbols;
    object_file.code_word_count = code_word_count;
    object_file.data_word_count = data_word_count;

    return object_file;
}

void update_entry_addresses(Object_File *object_file)
{
    Symbol *entry = object_file->entry_symbols;
    while (entry != NULL) {
        Symbol *defined_symbol = get_symbol(object_file->symbol_table, entry->symbol_name);
        if (defined_symbol != NULL) {
            entry->address = defined_symbol->address;
        }
        entry = entry->next_symbol;
    }
}

Object_File second_move(Object_File object_file)
{
    int i;
    Symbol *extern_calls = NULL;
    Compiled_Line *current_section = object_file.code_section;
    unsigned int symbol_address;
    unsigned int extern_address;
    Symbol *existing_extern;

    while (current_section != NULL) 
    {
        for (i = 0; i < current_section->num_of_words; i++)
        {
            if (strlen(current_section->missing_label[i]) > 0)
            {
                Symbol *symbol = get_symbol(object_file.symbol_table, current_section->missing_label[i]);
                if (symbol)
                {
                    if (symbol->symbol_opt == symbol_extern_def)
                    {
                        symbol_address = 1;
                        extern_address = current_section->begin_address + i;
                        
                        /* Check if this external symbol already exists in extern_calls */
                        existing_extern = get_symbol(extern_calls, current_section->missing_label[i]);
                        
                        if (existing_extern == NULL)
                        {
                            /* Create a new symbol for the first occurrence of this external reference */
                            extern_calls = insert_symbol_to_table(extern_calls, 
                                                                  current_section->missing_label[i], 
                                                                  current_section->line_index, 
                                                                  symbol_extern_def, 
                                                                  &extern_address);
                        }
                        else
                        {
                            /* Update the existing symbol with the new address */
                            Symbol *new_occurrence = create_symbol(current_section->missing_label[i], 
                                                                   current_section->line_index, 
                                                                   symbol_extern_def, 
                                                                   &extern_address);
                            /* Append the new occurrence to the end of the list */
                            while (existing_extern->next_symbol != NULL)
                            {
                                existing_extern = existing_extern->next_symbol;
                            }
                            existing_extern->next_symbol = new_occurrence;
                        }
                        
                        printf("Debug: External symbol %s used at address %u\n", current_section->missing_label[i], extern_address);
                    }
                    else
                    {
                        symbol_address = (symbol->address << 2) | 2;
                    }
                    current_section->words[i] = symbol_address;
                }
                else
                {
                    assembler_error(current_section->line_index, "Undefined symbol");
                }
            }
        }
        current_section = current_section->next_compiled_line;
    }

    update_entry_addresses(&object_file);
    object_file.extern_calls = extern_calls;
    return object_file;
}

int assembler(FILE *am_file, const char *am_filename)
{
    Object_File object_file;
    char entry_filename[MAX_FILE_NAME_LENGTH] = {0};
    char extern_filename[MAX_FILE_NAME_LENGTH] = {0};
    char ob_filename[MAX_FILE_NAME_LENGTH] = {0};

    printf("Debug: Starting first move\n");
    object_file = first_move(am_file, am_filename);
    printf("Debug: First move completed\n");

    printf("Debug: Starting second move\n");
    object_file = second_move(object_file);
    printf("Debug: Second move completed\n");

    if (object_file.entry_symbols != NULL)
    {
        printf("Debug: Creating .ent file\n");
        remove_suffix(am_filename, entry_filename, DOT_AM_SUFFIX);
        build_entry_file(entry_filename, object_file.entry_symbols);
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
    build_ob_file(ob_filename, object_file.code_section, object_file.data_section, 
                  object_file.code_word_count, object_file.data_word_count);

    free_symbol_table(object_file.symbol_table);
    free_symbol_table(object_file.entry_symbols);
    free_compiled_line_table(object_file.data_section);
    free_compiled_line_table(object_file.code_section);
    /* Note: extern_calls are freed in their respective build functions */

    return TRUE;
} 
