#include "analyzer_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Remove the str quotations to save it as is. */
char * remove_str_quotations(const char *str_content)
{
    char *cleaned_str;
    int len;

    if (str_content == NULL)
        return NULL;

    len = strlen(str_content);
    cleaned_str = (char *)malloc(len - 1); /* Allocate memory, excluding the quotes */
    if (cleaned_str == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    /* Copy characters excluding the surrounding quotes */
    strncpy(cleaned_str, str_content + 1, len - 2);
    cleaned_str[len - 2] = '\0'; /* Null-terminate the string */

    return cleaned_str;
}

/* Split two instruction's operands */
/* Split operands from instruction content. */
void split_operands(const char *inst_content, char *first_operand, char *second_operand)
{
    int i = 0;
    int j = 0;

    /* Skip leading whitespace */
    while (inst_content[i] == ' ' || inst_content[i] == '\t')
        i++;

    /* Copy first operand */
    while (inst_content[i] != ',' && inst_content[i] != '\0') 
    {
        if (j < MAX_OPERAND_LENGTH - 1)
            first_operand[j++] = inst_content[i++];
    }
    first_operand[j] = '\0';

    if (inst_content[i] == ',')
        i++; /* Skip the ',' */

    j = 0;

    /* Skip leading whitespace */
    while (inst_content[i] == ' ' || inst_content[i] == '\t')
        i++;

    /* Copy second operand */
    while (inst_content[i] != '\0') 
    {
        if (j < MAX_OPERAND_LENGTH - 1)
            second_operand[j++] = inst_content[i++];
    }
    
    second_operand[j] = '\0';
}


/* Get the number of a given register. */
/* Extract register number from the register string. */
int get_reg_num(const char *reg)
{
    int reg_num;
    char *reg_num_str;

    reg_num_str = strchr(reg, 'r');
    if (reg_num_str == NULL)
    {
        printf("Error: Invalid register format\n");
        exit(1);
    }
    reg_num_str += 1; /* Skip the 'r' */

    reg_num = str_to_int(reg_num_str);
    if (reg_num < 0 || reg_num > 7) /* Assuming 8 registers: r0 to r7 */
    {
        printf("Error: Register number out of range\n");
        exit(1);
    }

    return reg_num;
}


/* Checks if there is an error that has occurred until now regarding the current line. */
int is_valid_analyzed_line(const Analyzed_line *analyzed_line)
{
    if (strcmp(analyzed_line->syntax_error, "") == 0)
        return TRUE;

    return FALSE;
} 
