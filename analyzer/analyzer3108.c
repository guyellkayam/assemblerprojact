#include "analyzer.h"
#include "../helpers/analyzer_helper.h"
#include "../helpers/utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Struct that maps all the relevant Assembly Instructions. */
static asm_instruction asm_all_instructions[NUM_OF_INST] =
{
    /* {<inst_name>, <inst_key>, <immed_as_src>, <label_as_src>, <reg_as_src>, <immed_as_dest>, <label_as_dest>, <reg_as_dest>} */

    /* Require 2 operands */
    {"mov", inst_mov, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE},
    {"cmp", inst_cmp, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE},
    {"add", inst_add, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE},
    {"sub", inst_sub, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE},
    {"lea", inst_lea, FALSE, TRUE, FALSE, FALSE, TRUE, TRUE},

    /* Require 1 operand */
    {"not", inst_not, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"clr", inst_clr,  FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"inc", inst_inc, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"dec", inst_dec, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"jmp", inst_jmp, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"bne", inst_bne, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"red", inst_red, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},
    {"prn", inst_prn, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE},
    {"jsr", inst_jsr, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE},

    /* Without operands */
    {"rts", inst_rts, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
    {"stop", inst_stop, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE},
};

/* Struct that maps all the relevant Assembly Directives. */
static asm_directive asm_all_directives[NUM_OF_DIR] = 
{
    /* {<dir_name>, <dir_key>} */
    {".data", dir_data},
    {".string", dir_string},
    {".extern", dir_extern},
    {".entry", dir_entry},
};

void set_main_label(char *line, Analyzed_line *analyzed_line)
{
    char label_name[MAX_LINE_LENGTH];
    char clean_line[MAX_LINE_LENGTH];
    int i;

    remove_white_spaces(line, clean_line);
    memset(label_name, '\0', MAX_LINE_LENGTH);

    if (strchr(clean_line, ':') == NULL) 
    {
        strcpy(analyzed_line->label_name, label_name);
    }
    else 
    {
        for (i = 0; clean_line[i] != ':'; i++) 
        {
            label_name[i] = clean_line[i];
        }
        label_name[i] = '\0';
        strcpy(analyzed_line->label_name, label_name);
    }
}

void set_dir_or_inst(char *line, Analyzed_line *analyzed_line)
{
    int i;
    char clean_line[MAX_LINE_LENGTH];

    remove_white_spaces(line, clean_line);

    for (i = 0; i < NUM_OF_DIR; i++) 
    {
        if (strstr(clean_line, asm_all_directives[i].dir_name) != NULL) 
        {
            analyzed_line->analyzed_line_opt = directive;
            analyzed_line->dir_or_inst.directive.dir_opt = asm_all_directives[i].dir_key;
            return;
        }
    }

    for (i = 0; i < NUM_OF_INST; i++)
    {
        if (strstr(clean_line, asm_all_instructions[i].inst_name) != NULL)
        {
            analyzed_line->analyzed_line_opt = instruction;
            analyzed_line->dir_or_inst.instruction.inst_opt = asm_all_instructions[i].inst_key;
            return;
        }
    }

    sprintf(analyzed_line->syntax_error, "The current line is not a valid Assembly instruction/directive!");
}

void set_ent_ext_label(char *line, Analyzed_line *analyzed_line)
{
    char clean_line[MAX_LINE_LENGTH];
    char *label_name;

    remove_white_spaces(line, clean_line);

    if (analyzed_line->dir_or_inst.directive.dir_opt == dir_extern)
    {
        label_name = strstr(clean_line, DOT_EXT_AS_STRING);
        if (label_name != NULL)
        {
            label_name += strlen(DOT_EXT_AS_STRING);
            while (*label_name == ' ' || *label_name == '\t') label_name++; /* Skip whitespace */
            strcpy(analyzed_line->dir_or_inst.directive.dir_operand.label_name, label_name);
        }
    }
    
    else if(analyzed_line->dir_or_inst.directive.dir_opt == dir_entry)
    {
        label_name = strstr(clean_line, DOT_ENT_AS_STRING);
        if (label_name != NULL)
        {
            label_name += strlen(DOT_ENT_AS_STRING);
            strcpy(analyzed_line->dir_or_inst.directive.dir_operand.label_name, label_name);
        }
    }
}

void set_dir_string(char *line, Analyzed_line *analyzed_line)
{
    char clean_line[MAX_LINE_LENGTH];
    char *string_start;
    char *string_end;
    
    remove_white_spaces(line, clean_line);
    printf("Debug: Processing .string line: '%s'\n", clean_line);

    /* Find the start of the string content */
    string_start = strstr(clean_line, ".string");
    if (string_start == NULL)
    {
        printf("Debug: .string directive not found\n");
        sprintf(analyzed_line->syntax_error, "Invalid '.string' directive syntax!");
        return;
    }
    
    /* Move past ".string" */
    string_start += 7;  /* length of ".string" */
    
    /* Find the first quotation mark */
    while (*string_start != '"' && *string_start != '\0')
    {
        string_start++;
    }
    
    if (*string_start != '"')
    {
        printf("Debug: No opening quotation mark found\n");
        sprintf(analyzed_line->syntax_error, "Invalid '.string' content syntax! Opening quotation is missing!");
        return;
    }
    
    string_start++;  /* Move past the opening quote */

    /* Find the end of the string content */
    string_end = strchr(string_start, '"');
    if (string_end == NULL)
    {
        printf("Debug: No closing quotation mark found\n");
        sprintf(analyzed_line->syntax_error, "Invalid '.string' content syntax! Closing quotation is missing!");
        return;
    }

    /* Extract the string content */
    *string_end = '\0';  /* Temporarily terminate the string at the closing quote */

    if (*string_start == '\0')
    {
        printf("Debug: Empty string\n");
        sprintf(analyzed_line->syntax_error, "Invalid '.string' content syntax! Empty string!");
        return;
    }

    strncpy(analyzed_line->dir_or_inst.directive.dir_operand.string, string_start, MAX_STR_LENGTH - 1);
    analyzed_line->dir_or_inst.directive.dir_operand.string[MAX_STR_LENGTH - 1] = '\0';  /* Ensure null-termination */
    printf("Debug: Successfully processed string: '%s'\n", analyzed_line->dir_or_inst.directive.dir_operand.string);
}

void set_dir_data(char *line, Analyzed_line *analyzed_line)
{
    int i = 0;
    char clean_line[MAX_LINE_LENGTH];
    char *data_content_as_string;
    long int num;
    int num_conversion_result;

    remove_white_spaces(line, clean_line);

    data_content_as_string = strstr(clean_line, DOT_DATA_AS_STRING);
    data_content_as_string += strlen(DOT_DATA_AS_STRING);

    while (sscanf(data_content_as_string, "%ld%n", &num, &num_conversion_result) == 1) 
    {
        if (num_conversion_result == 0 || data_content_as_string[num_conversion_result] != ',')
        {
            if (data_content_as_string[num_conversion_result] != '\0')
            {
                sprintf(analyzed_line->syntax_error, "Invalid data content! .data content must be an integer!");
                return;
            }
        }

        data_content_as_string = strchr(data_content_as_string, ',');
        analyzed_line->dir_or_inst.directive.dir_operand.data.data[i] = num;
        i++;
        analyzed_line->dir_or_inst.directive.dir_operand.data.data_count = i;

        if (data_content_as_string == NULL)
            break;
        
        data_content_as_string += 1;
    }

    if (data_content_as_string != NULL)
    {
        sprintf(analyzed_line->syntax_error, "Invalid data content! .data content must be an integer!");
    }
}

void set_directive(char *line, Analyzed_line *analyzed_line) 
{
    printf("Debug: Directive type: %d\n", analyzed_line->dir_or_inst.directive.dir_opt);  

    if (analyzed_line->dir_or_inst.directive.dir_opt == dir_entry || analyzed_line->dir_or_inst.directive.dir_opt == dir_extern)
    {
        set_ent_ext_label(line, analyzed_line);
    }
    else if (analyzed_line->dir_or_inst.directive.dir_opt == dir_string)
    {
        set_dir_string(line, analyzed_line);
    }
    else if (analyzed_line->dir_or_inst.directive.dir_opt == dir_data)
    {
        set_dir_data(line, analyzed_line);
    }
}

const char *get_inst_name(int inst_enum_code)
{
    int i;
    
    for (i = 0; i < NUM_OF_INST; i++)
    {
        if (inst_enum_code == asm_all_instructions[i].inst_key) 
        {
            return asm_all_instructions[i].inst_name;
        }
    }

    return NULL;
}

char *get_inst_content(const char *inst_name, char *clean_line)
{
    char *found;
    size_t len;

    found = strstr(clean_line, inst_name);
    if (found == NULL)
        return NULL;

    found += strlen(inst_name);
    len = strlen(found) + 1;

    return strncpy((char*) malloc(len), found, len);
}

int get_num_inst_operands(int inst_enum_code)
{
    if (inst_enum_code <= inst_lea)
        return TWO_OPERANDS;
    else if (inst_enum_code <= inst_jsr)
        return SINGLE_OPERAND;
    else
        return ZERO_OPERANDS;
}

void set_inst_operand(char *inst_operand, Analyzed_line *analyzed_line, int operand_i)
{
    char *endptr;
    long value;
    int is_indirect = 0;

    remove_white_spaces(inst_operand, inst_operand);

    if (inst_operand[0] == '*')
    {
        is_indirect = 1;
        inst_operand++; /* Move past the '*' */
    }

    if (inst_operand[0] == 'r' && strlen(inst_operand) == 2)
    {
        int reg_num = inst_operand[1] - '0';
        if (reg_num >= MIN_REG_NUM && reg_num <= MAX_REG_NUM)
        {
            analyzed_line->dir_or_inst.instruction.inst_operand_options[operand_i] = 
                is_indirect ? operand_register_indirect : operand_register;
            analyzed_line->dir_or_inst.instruction.inst_operands[operand_i].register_number = reg_num;
            return;
        }
    }
    else if (inst_operand[0] == '#')
    {
        if (is_indirect)
        {
            sprintf(analyzed_line->syntax_error, "Invalid indirect addressing for immediate value: %s", inst_operand);
            return;
        }
        value = strtol(inst_operand + 1, &endptr, 10);
        if (*endptr == '\0')
        {
            analyzed_line->dir_or_inst.instruction.inst_operand_options[operand_i] = operand_const_number;
            analyzed_line->dir_or_inst.instruction.inst_operands[operand_i].const_number = (int)value;
            return;
        }
    }
    else if (is_valid_label(inst_operand))
    {
        analyzed_line->dir_or_inst.instruction.inst_operand_options[operand_i] = 
            is_indirect ? operand_label_indirect : operand_label;
        strncpy(analyzed_line->dir_or_inst.instruction.inst_operands[operand_i].label, inst_operand, MAX_LABEL_LENGTH - 1);
        analyzed_line->dir_or_inst.instruction.inst_operands[operand_i].label[MAX_LABEL_LENGTH - 1] = '\0';
        return;
    }

    sprintf(analyzed_line->syntax_error, "Invalid operand: %s", inst_operand);
}

void set_instruction(char *line, Analyzed_line *analyzed_line) {
    const char *inst_name = get_inst_name(analyzed_line->dir_or_inst.instruction.inst_opt);
    char *inst_content = get_inst_content(inst_name, line);
    char *operand1, *operand2;
    int num_operands;

    if (inst_content == NULL) {
        sprintf(analyzed_line->syntax_error, "Invalid instruction syntax!");
        return;
    }

    num_operands = get_num_inst_operands(analyzed_line->dir_or_inst.instruction.inst_opt);
    printf("Debug: Instruction %s has %d operands\n", inst_name, num_operands);

    if (num_operands == TWO_OPERANDS) {
        operand1 = strtok(inst_content, ",");
        operand2 = strtok(NULL, ",");
        if (operand1 == NULL || operand2 == NULL) {
            sprintf(analyzed_line->syntax_error, "Two operands expected for instruction %s", inst_name);
            free(inst_content);
            return;
        }
        set_inst_operand(operand1, analyzed_line, 0);
        set_inst_operand(operand2, analyzed_line, 1);
        printf("Debug: Two operands set - Op1: %s, Op2: %s\n", operand1, operand2);
    } else if (num_operands == SINGLE_OPERAND) {
        operand1 = strtok(inst_content, ",");
        if (operand1 == NULL) {
            sprintf(analyzed_line->syntax_error, "One operand expected for instruction %s", inst_name);
            free(inst_content);
            return;
        }
        set_inst_operand(operand1, analyzed_line, 0);
        printf("Debug: Single operand set - Op: %s\n", operand1);
    } else if (num_operands != ZERO_OPERANDS) {
        sprintf(analyzed_line->syntax_error, "Unexpected number of operands for instruction %s", inst_name);
    }

    free(inst_content);
}

Analyzed_line analyze_line(char *line)
{
    Analyzed_line analyzed_line;
    char clean_line[MAX_LINE_LENGTH];

    memset(&analyzed_line, 0, sizeof(Analyzed_line));
    remove_white_spaces(line, clean_line);

    printf("Debug: Analyzing line: '%s'\n", clean_line);

    if (strlen(clean_line) == 0)
    {
        strcpy(analyzed_line.syntax_error, "Empty line!");
        return analyzed_line;
    }

    set_main_label(clean_line, &analyzed_line);
    set_dir_or_inst(clean_line, &analyzed_line);

    if (analyzed_line.analyzed_line_opt == directive)
    {
        printf("Debug: Line is a directive\n");
        printf("Debug: Line before set_directive: '%s'\n", clean_line); 
        set_directive(clean_line, &analyzed_line);
    }
    else if (analyzed_line.analyzed_line_opt == instruction)
    {
        printf("Debug: Line is an instruction\n");
        set_instruction(clean_line, &analyzed_line);
    }
    
    if (analyzed_line.syntax_error[0] != '\0') {
        printf("Debug: Syntax error in line: %s\n", analyzed_line.syntax_error);
    }

    return analyzed_line;
}

Analyzed_line get_analyzed_line(char *line) {
    return analyze_line(line);
} 

