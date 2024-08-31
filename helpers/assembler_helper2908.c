#include "assembler_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set extra words for instruction based on operand types. */
void set_inst_extra_words(const Analyzed_line *analyzed_line, Compiled_Line *compiled_line, int num_of_operands, unsigned int *address) {
    unsigned int extra_word = 0;
    int i;

    for (i = 0; i < num_of_operands; i++) 
    {
        /* Const Number */
        if (analyzed_line->dir_or_inst.instruction.inst_operand_options[i] == operand_const_number)
        {
            extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[i].const_number << ARE_INDENTATION;
            insert_word(compiled_line, extra_word, address);
            printf("Debug: Added constant number word: %u\n", extra_word);
        }

        /* Register */
        if (analyzed_line->dir_or_inst.instruction.inst_operand_options[i] == operand_register)
        {
            if (num_of_operands == 2 && 
                analyzed_line->dir_or_inst.instruction.inst_operand_options[0] == operand_register &&
                analyzed_line->dir_or_inst.instruction.inst_operand_options[1] == operand_register) 
            {
                extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[0].register_number << SRC_REG_INDENTATION; /* SRC */
                extra_word |= analyzed_line->dir_or_inst.instruction.inst_operands[1].register_number << DEST_REG_INDENTATION; /* DEST */
                insert_word(compiled_line, extra_word, address);
                printf("Debug: Added two-register word: %u\n", extra_word);
                return;
            }
            else if (i == 0)
            {
                extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[i].register_number << SRC_REG_INDENTATION;
                insert_word(compiled_line, extra_word, address);
                printf("Debug: Added source register word: %u\n", extra_word);
            }
            else 
            {
                extra_word = analyzed_line->dir_or_inst.instruction.inst_operands[i].register_number << DEST_REG_INDENTATION;
                insert_word(compiled_line, extra_word, address);
                printf("Debug: Added destination register word: %u\n", extra_word);
            }
        }

        /* Label */
        if (analyzed_line->dir_or_inst.instruction.inst_operand_options[i] == operand_label)
        {
            if (num_of_operands == 1)
            {
                compiled_line->missing_label_op_type[0] = dest_label;
                strncpy(compiled_line->missing_label[0], analyzed_line->dir_or_inst.instruction.inst_operands[0].label, MAX_LABEL_LENGTH - 1);
                compiled_line->missing_label[0][MAX_LABEL_LENGTH - 1] = '\0';
                insert_word(compiled_line, 0, address); /* Dummy. Will be changed later */
                printf("Debug: Set label for single operand instruction: %s\n", compiled_line->missing_label[0]);
                return;
            }

            compiled_line->missing_label_op_type[i] = i;
            strncpy(compiled_line->missing_label[i], analyzed_line->dir_or_inst.instruction.inst_operands[i].label, MAX_LABEL_LENGTH - 1);
            compiled_line->missing_label[i][MAX_LABEL_LENGTH - 1] = '\0';
            insert_word(compiled_line, 0, address); /* Dummy. Will be changed later */
            printf("Debug: Set label for operand %d: %s\n", i, compiled_line->missing_label[i]);
        }
    }
}


/* Generate assembler warning */
void assembler_warning(int line_index, const char *message)
{
    printf("WARNING! Line Number '%d': %s\n", line_index, message);
}

/* Generate assembler error */
void assembler_error(int line_index, const char *message)
{
    printf("Error! Exit in Line Number '%d': %s\n", line_index, message);
    exit(1);


}
