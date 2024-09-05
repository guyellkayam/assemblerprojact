#include "assembler_helper.h"
#include "../analyzer/analyzer.h"
#include "../data_structures/compiled_line.h"
#include "../helpers/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARE_INDENTATION 2
#define SRC_REG_INDENTATION 8
#define DEST_REG_INDENTATION 2

void set_inst_extra_words(const Analyzed_line *analyzed_line, Compiled_Line *compiled_line,
                          int num_of_operands, unsigned int *address, unsigned int *code_word_count) 
{
    unsigned int extra_word = 0;
    int i;

    if (analyzed_line == NULL || compiled_line == NULL || address == NULL || code_word_count == NULL) {
        fprintf(stderr, "Error: Null pointer passed to set_inst_extra_words\n");
        return;
    }

    for (i = 0; i < num_of_operands; i++) 
    {
        switch (analyzed_line->dir_or_inst.instruction.inst_operand_options[i])
        {
            case operand_const_number:
                extra_word = (analyzed_line->dir_or_inst.instruction.inst_operands[i].const_number & 0xFFF) << ARE_INDENTATION;
                extra_word |= 4; /* ARE bits for absolute value */
                insert_word(compiled_line, extra_word, address);
                (*code_word_count)++;
                printf("Debug: Added constant number word: %05o\n", extra_word);
                break;

            case operand_register:
            case operand_register_indirect:
                if (num_of_operands == 2 && 
                    analyzed_line->dir_or_inst.instruction.inst_operand_options[0] == operand_register &&
                    analyzed_line->dir_or_inst.instruction.inst_operand_options[1] == operand_register) 
                {
                    extra_word = (analyzed_line->dir_or_inst.instruction.inst_operands[0].register_number << SRC_REG_INDENTATION) |
                                 (analyzed_line->dir_or_inst.instruction.inst_operands[1].register_number << DEST_REG_INDENTATION);
                    extra_word |= 4; /* ARE bits for absolute value */
                    insert_word(compiled_line, extra_word, address);
                    (*code_word_count)++;
                    printf("Debug: Added two-register word: %05o\n", extra_word);
                    return;
                }
                else if (i == 0)
                {
                    extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[i].register_number << SRC_REG_INDENTATION;
                }
                else 
                {
                    extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[i].register_number << DEST_REG_INDENTATION;
                }
                extra_word |= 4; /* ARE bits for absolute value */
                insert_word(compiled_line, extra_word, address);
                (*code_word_count)++;
                printf("Debug: Added %s register word: %05o\n", (i == 0) ? "source" : "destination", extra_word);
                break;

            case operand_label:
            case operand_label_indirect:
                if (compiled_line->num_of_words >= 2) {
                    fprintf(stderr, "Error: Too many words for instruction\n");
                    return;
                }
                compiled_line->missing_label_op_type[compiled_line->num_of_words] = i;
                strncpy(compiled_line->missing_label[compiled_line->num_of_words], 
                        analyzed_line->dir_or_inst.instruction.inst_operands[i].label, 
                        MAX_LABEL_LENGTH - 1);
                compiled_line->missing_label[compiled_line->num_of_words][MAX_LABEL_LENGTH - 1] = '\0';
                insert_word(compiled_line, 0, address); /* Placeholder to be resolved in second pass */
                (*code_word_count)++;
                printf("Debug: Added label placeholder for: %s\n", compiled_line->missing_label[compiled_line->num_of_words - 1]);
                break;

            default:
                fprintf(stderr, "Debug: Unknown operand type %d for operand %d\n", 
                       analyzed_line->dir_or_inst.instruction.inst_operand_options[i], i);
                assembler_error(*address, "Unknown operand type");
                return;
        }
    }
}

void assembler_warning(int line_index, const char *message) {
    fprintf(stderr, "Assembler Warning at line %d: %s\n", line_index, message);
}

void assembler_error(int line_index, const char *message) {
    fprintf(stderr, "Assembler Error at line %d: %s\n", line_index, message);
    exit(1);
} 
