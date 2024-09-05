#ifndef ANALYZER_H
#define ANALYZER_H

#include "../helpers/utils.h"
#include "../data_structures/compiled_line.h"

#define NUM_OF_INST 16
#define NUM_OF_DIR 4
#define MAX_SYTX_ERR_LENGTH 200
#define MAX_STR_LENGTH 40

#define TWO_OPERANDS 2
#define SINGLE_OPERAND 1
#define ZERO_OPERANDS 0

#define MIN_REG_NUM 0
#define MAX_REG_NUM 7

#define DOT_ENT_AS_STRING ".entry"
#define DOT_EXT_AS_STRING ".extern"
#define DOT_STR_AS_STRING ".string"
#define DOT_DATA_AS_STRING ".data"

/* Ast that defines a Directive / Instruction in Assembly */
typedef struct Analyzed_line
{
    char syntax_error[MAX_SYTX_ERR_LENGTH];
    char label_name[MAX_LABEL_LENGTH]; 

    /* Only one of the following can be the line type. Blank and Comment lines are in the preprocessor responsibility. */
    enum
    {
        directive,
        instruction
    } analyzed_line_opt;

    union
    {
        struct /* Directive */
        {
            enum /* Four kinds of directive */
            {
                dir_extern,
                dir_entry,
                dir_string,
                dir_data

            } dir_opt;

            union
            {
                char label_name[MAX_LABEL_LENGTH]; /* .entry or .extern */
                char string[MAX_STR_LENGTH]; /* .string */
                struct /* .data */
                {
                    int data[MAX_LINE_LENGTH];
                    int data_count;
                } data;

            } dir_operand;

        } directive; /* End of Directive */

        struct /* Instruction */
        {
            enum /* 16 opcodes */
            {
                /* Require 2 operands */
                inst_mov,
                inst_cmp,
                inst_add,
                inst_sub,
                inst_lea,

                /* Require 1 operand */
                inst_not,
                inst_clr,
                inst_inc,
                inst_dec,
                inst_jmp,
                inst_bne,
                inst_red,
                inst_prn,
                inst_jsr,

                /* Without operands */
                inst_rts,
                inst_stop

            } inst_opt;

            enum /* Addressing modes */
            {
                operand_none = 0,
                operand_const_number = 1,
                operand_label = 2,
                operand_register = 3,
                operand_register_indirect = 4,
                operand_label_indirect = 5

            } inst_operand_options[2];

            union
            {
                int const_number;
                int register_number;
                char label[MAX_LABEL_LENGTH];

            } inst_operands[2]; /* 2 for source and dest */

        } instruction; /* End of Instruction */

    } dir_or_inst;

    int needs_label_resolution[2];  /* For up to 2 operands */
} Analyzed_line;

/* Struct that defines an Assembly Instruction and the relevant data. */
typedef struct asm_instruction
{
    const char *inst_name;
    int inst_key;

    /* Allowed addressing modes for the src operand */
    int immed_as_src;
    int label_as_src;
    int reg_as_src;

    /* Allowed addressing modes for the dest operand */
    int immed_as_dest;
    int label_as_dest;
    int reg_as_dest;

} asm_instruction;

/* Struct that defines an Assembly directive and the relevant data. */
typedef struct asm_directive
{
    const char *dir_name;
    int dir_key;

} asm_directive;

void set_main_label(char *line, Analyzed_line *analyzed_line);
void set_dir_or_inst(char *line, Analyzed_line *analyzed_line);
void set_ent_ext_label(char *line, Analyzed_line *analyzed_line);
void set_dir_string(char *line, Analyzed_line *analyzed_line);
void set_dir_data(char *line, Analyzed_line *analyzed_line);
void set_directive(char *line, Analyzed_line *analyzed_line);
const char *get_inst_name(int inst_enum_code);
char *get_inst_content(const char *inst_name, char *clean_line);
int get_num_inst_operands(int inst_enum_code);
void set_inst_operand(char *inst_operand, Analyzed_line *analyzed_line, int operand_i);
void set_instruction(char *line, Analyzed_line *analyzed_line);
Analyzed_line analyze_line(char *line);
Analyzed_line get_analyzed_line(char *line);

#endif /* ANALYZER_H */ 
