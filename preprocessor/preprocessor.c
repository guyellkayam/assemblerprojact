#include "preprocessor.h"
#include "macro_table.h"
#include "../helpers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_macro_def(char *line)
{
    char clean_line[MAX_LINE_LENGTH];
    remove_prefix_white_spaces(line, clean_line);

    return strncmp(clean_line, "mcro ", 5) == 0;
}

int is_macro_call(Macro *macro, char *line)
{
    char clean_line[MAX_LINE_LENGTH];
    remove_white_spaces(line, clean_line);

    while (macro != NULL)
    {
        if (strcmp(macro->name, clean_line) == 0)
            return 1;

        macro = macro->next_macro;
    }

    return 0;
}

void deploy_macro(FILE *output_file, Macro *macro, char *line)
{
    int i;
    char macro_name[MAX_LINE_LENGTH];
    Macro *macro_to_deploy;

    remove_white_spaces(line, macro_name);

    macro_to_deploy = get_macro(macro, macro_name);
    if (macro_to_deploy != NULL)
    {
        for (i = 0; i < macro_to_deploy->num_of_lines; i++)
        {
            char clean_macro_line[MAX_LINE_LENGTH];
            remove_macro_indentation(macro_to_deploy->lines[i], clean_macro_line);
            fputs(clean_macro_line, output_file);
        }
    }
}

enum line_type get_line_type(Macro *macro, char *line)
{
    char clean_line[MAX_LINE_LENGTH];
    remove_white_spaces(line, clean_line);

    if (*clean_line == '\0')
        return blank;

    if (*clean_line == COMMENT_PREFIX)
        return comment;

    if (strcmp(clean_line, "endmcro") == 0)
        return macro_end;

    if (is_macro_def(line))
        return macro_def;

    if (is_macro_call(macro, line))
        return macro_call;

    return any_other_line;
}

char *get_macro_name_from_line(char *line)
{
    char clean_line[MAX_LINE_LENGTH];
    char *macro_name;

    remove_white_spaces(line, clean_line);

    macro_name = (char *)malloc(MAX_LINE_LENGTH);
    if (macro_name == NULL)
    {
        printf("Error allocating memory for macro_name in get_macro_name_from_line.\n");
        exit(1);
    }

    strncpy(macro_name, clean_line + MACRO_DEF_STR_LENGTH, MAX_LINE_LENGTH - MACRO_DEF_STR_LENGTH);
    return macro_name;
}

char *process_as_file(char *filename)
{
    FILE *input_file;
    FILE *output_file;
    int is_macro_on = 0;
    Macro *macro_table = NULL;
    Macro *macro_on = NULL;
    char line[MAX_LINE_LENGTH];
    char input_filename[MAX_FILE_NAME_LENGTH];
    char output_filename[MAX_FILE_NAME_LENGTH];
    char *result_filename;
    enum line_type current_line_type;

    printf("Processing file: %s\n", filename);

    snprintf(input_filename, sizeof(input_filename), "%s.as", filename);
    snprintf(output_filename, sizeof(output_filename), "%s.am", filename);

    printf("Attempting to open input file: %s\n", input_filename);
    input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        perror("Error opening the input file");
        return NULL;
    }

    printf("Attempting to open output file: %s\n", output_filename);
    output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        perror("Error opening the output file");
        fclose(input_file);
        return NULL;
    }

    printf("Successfully opened both input and output files.\n");

    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        current_line_type = get_line_type(macro_table, line);

        if (current_line_type == blank || current_line_type == comment)
        {
            continue;
        }

        if (current_line_type == macro_end)
        {
            is_macro_on = 0;
        }
        else if (is_macro_on)
        {
            insert_macro_line(macro_on, line);
            continue;
        }
        else if (current_line_type == macro_def)
        {
            char *macro_name = get_macro_name_from_line(line);
            macro_table = insert_macro_to_table(macro_table, macro_name);
            macro_on = get_macro(macro_table, macro_name);
            free(macro_name);
            is_macro_on = 1;
        }
        else if (current_line_type == macro_call)
        {
            deploy_macro(output_file, macro_table, line);
            continue;
        }
        else
        {
            fputs(line, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
    free_macro_table(macro_table);

    result_filename = strdup(output_filename);
    if (result_filename == NULL)
    {
        printf("Memory allocation failed for result_filename.\n");
        exit(1);
    }

    return result_filename;
}
