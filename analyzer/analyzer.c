#include "analyzer.h"
#include "../helpers/analyzer_helper.h"
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

/* Checks if there is a label definition in the line. If yes, set it to the label's name, otherwise set to NULL. */
void set_main_label(char *line, Analyzed_line *analyzed_line)
{
    char label_name[MAX_LINE_LENGTH];
    char clean_line[MAX_LINE_LENGTH];
    int i;

    remove_white_spaces(line, clean_line);

    /* Initialize label_name with null characters. */
    memset(label_name, '\0', MAX_LINE_LENGTH);

    /* There is no label definition in this line. */
    if (strchr(clean_line, ':') == NULL) 
    {
        /* Set the label_name to NULL. */
        strcpy(analyzed_line->label_name, label_name);
    }
    else 
    {
        /* Get the label name from the line */
        for (i = 0; clean_line[i] != ':'; i++) 
        {
            label_name[i] = clean_line[i];
        }

        label_name[i] = '\0';

        /* Set the label_name */
        strcpy(analyzed_line->label_name, label_name);
    }
}

/* Checks and sets if the current line is an Assembly instruction or directive. */
void set_dir_or_inst(char *line, Analyzed_line *analyzed_line)
{
    int i;
    char clean_line[MAX_LINE_LENGTH];

    remove_white_spaces(line, clean_line);

    /* Directive */
    for (i = 0; i < NUM_OF_DIR; i++) 
    {
        if (strstr(clean_line, asm_all_directives[i].dir_name) != NULL) 
        {
            analyzed_line->analyzed_line_opt = directive;
            analyzed_line->dir_or_inst.directive.dir_opt = asm_all_directives[i].dir_key;
            return;
        }
    }

    /* Instruction */
    for (i = 0; i < NUM_OF_INST; i++)
    {
        if (strstr(clean_line, asm_all_instructions[i].inst_name) != NULL)
        {
            analyzed_line->analyzed_line_opt = instruction;
            analyzed_line->dir_or_inst.instruction.inst_opt = asm_all_instructions[i].inst_key;
            return;
        }
    }

    /* Not dir or inst. */
    sprintf(analyzed_line->syntax_error, "The current line is not a valid Assembly instruction/directive!");
}

/* Set the label name of a '.entry' / '.extern' Assembly directive. */
void set_ent_ext_label(char *line, Analyzed_line *analyzed_line)
{
    char clean_line[MAX_LINE_LENGTH];
    char *label_name;

    remove_white_spaces(line, clean_line);

    /* Get extern label name */
    if (analyzed_line->dir_or_inst.directive.dir_opt == dir_extern)
    {
        label_name = strstr(clean_line, DOT_EXT_AS_STRING);
        if (label_name == NULL)
            return;
            
        label_name += strlen(DOT_EXT_AS_STRING); /* Skip the '.extern' string */
        strcpy(analyzed_line->dir_or_inst.directive.dir_operand.label_name, label_name);
    }
    /* Get entry label name */     
    else if(analyzed_line->dir_or_inst.directive.dir_opt == dir_entry)
    {
        label_name = strstr(clean_line, DOT_ENT_AS_STRING);
        if (label_name == NULL)
            return;
        
        label_name += strlen(DOT_ENT_AS_STRING); /* Skip the '.entry' string */
        strcpy(analyzed_line->dir_or_inst.directive.dir_operand.label_name, label_name);
    }
}

/* Set the string of a '.string' Assembly directive. */
void set_dir_string(char *line, Analyzed_line *analyzed_line)
{
    char clean_line[MAX_LINE_LENGTH];
    char *string_content;

    remove_white_spaces(line, clean_line);

    string_content = strchr(clean_line, '\"');
    if (string_content == NULL)
    {
        sprintf(analyzed_line->syntax_error, "Invalid '.string' content syntax! Quotations are missing!");
        return;
    }

    string_content = remove_str_quotations(string_content);
    if (string_content[0] == '\0')
    {
        sprintf(analyzed_line->syntax_error, "Invalid '.string' content syntax! The closing quotation is missing!");
        return;
    }

    strcpy(analyzed_line->dir_or_inst.directive.dir_operand.string, string_content);
}

/* Set the data of a '.data' Assembly directive. */
void set_dir_data(char *line, Analyzed_line *analyzed_line)
{
    int i = 0;
    char clean_line[MAX_LINE_LENGTH];
    char *data_content_as_string;
    long int num;
    int num_conversion_result;

    remove_white_spaces(line, clean_line);

    data_content_as_string = strstr(clean_line, DOT_DATA_AS_STRING);
    data_content_as_string += strlen(DOT_DATA_AS_STRING); /* Skip the '.data' string */

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

    /* Catch Strings / Chars */
    if (data_content_as_string != NULL)
    {
        sprintf(analyzed_line->syntax_error, "Invalid data content! .data content must be an integer!");
    }
}

/* Set an Assembly directive according to the directive type. */
void set_directive(char *line, Analyzed_line *analyzed_line) 
{
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

/* Get the instruction name from the table based on the instruction enum code. */
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

    /* Error */
    return NULL;
}

/* Get the instruction content - the data that comes after the opcode. */
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

/* Analyze a line of Assembly code. */
Analyzed_line analyze_line(char *line)
{
    Analyzed_line analyzed_line;
    char clean_line[MAX_LINE_LENGTH];

    /* Initialize Analyzed_line structure. */
    memset(&analyzed_line, 0, sizeof(Analyzed_line));

    /* Remove leading/trailing white spaces from the line. */
    remove_white_spaces(line, clean_line);

    /* Check if the line is empty. */
    if (strlen(clean_line) == 0)
    {
        strcpy(analyzed_line.syntax_error, "Empty line!");
        return analyzed_line;
    }

    /* Set the label name if it exists. */
    set_main_label(clean_line, &analyzed_line);

    /* Determine whether the line contains a directive or instruction. */
    set_dir_or_inst(clean_line, &analyzed_line);

    /* Set the content for directives. */
    if (analyzed_line.analyzed_line_opt == directive)
    {
        set_directive(clean_line, &analyzed_line);
    }
    
    return analyzed_line;
}
