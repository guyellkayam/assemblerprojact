#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Print to the terminal a decimal number in binary base (15 bits). */
void print_terminal_decimal_to_binary(int decimal)
{
    int binary[15];
    int i;
    int index = 0;

    if (decimal == 0)
    {
        printf("Binary: 0\n");
        return;
    }

    if (decimal < 0)
    {
        printf("Binary: ");
        decimal = (1 << 15) + decimal; /* Adjust for 15-bit two's complement */
    }
    else
    {
        printf("Binary: ");
    }

    while (index < 15)
    {
        binary[index++] = decimal % 2;
        decimal /= 2;
    }

    for (i = 14; i >= 0; i--)
    {
        printf("%d", binary[i]);
    }

    printf("\n");
}

/* Print to an output file a decimal number in binary base (15 bits). */
void print_file_decimal_to_binary(int decimal, FILE * output_file)
{
    int binary[15];
    int i;
    int index = 0;

    if (decimal == 0)
    {
        fprintf(output_file, "Binary: 0\n");
        return;
    }

    if (decimal < 0)
    {
        fprintf(output_file, "Binary: ");
        decimal = (1 << 15) + decimal; /* Adjust for 15-bit two's complement */
    }
    else
    {
        fprintf(output_file, "Binary: ");
    }

    while (index < 15)
    {
        binary[index++] = decimal % 2;
        decimal /= 2;
    }

    for (i = 14; i >= 0; i--)
    {
        fprintf(output_file, "%d", binary[i]);
    }

    fprintf(output_file, "\n");
}

/* Check if a string is a valid label */
int is_valid_label(const char *str)
{
    int i;
    
    /* Check if the string is NULL or empty */
    if (str == NULL || *str == '\0')
        return FALSE;
    
    /* Check if the first character is alphabetic */
    if (!isalpha(str[0]))
        return FALSE;
    
    /* Check the length of the label */
    if (strlen(str) > MAX_LABEL_LENGTH)
        return FALSE;
    
    /* Check if all characters are alphanumeric or underscore */
    for (i = 1; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]) && str[i] != '_')
            return FALSE;
    }
    
    /* TODO: Add a check for reserved words if necessary */
    
    return TRUE;
}

void process_string_directive(char *line)
{
    char *start_quote, *end_quote;

    /* Find the opening quotation mark */
    start_quote = strchr(line, '\"');
    if (start_quote == NULL) {
        printf("Debug: No opening quotation mark found\n");
        printf("Syntax error in line: Invalid '.string' content syntax! Opening quotation is missing!\n");
        exit(1);
    }

    /* Find the closing quotation mark */
    end_quote = strrchr(line, '\"');
    if (end_quote == NULL || end_quote == start_quote) {
        printf("Debug: No closing quotation mark found\n");
        printf("Syntax error in line: Invalid '.string' content syntax! Closing quotation is missing!\n");
        exit(1);
    }

    printf("Debug: Processing .string line: '%s'\n", line);
    /* Further processing of the .string directive can be done here */
}

void remove_white_spaces(char *source_line, char *dest_line)
{
    int start = 0, end = strlen(source_line) - 1;
    int i, j;

    /* Trim leading white spaces */
    while (isspace((unsigned char)source_line[start])) {
        start++;
    }

    /* Trim trailing white spaces */
    while (end >= start && isspace((unsigned char)source_line[end])) {
        end--;
    }

    /* Copy the trimmed content to dest_line */
    for (i = start, j = 0; i <= end; i++, j++) {
        dest_line[j] = source_line[i];
    }
    dest_line[j] = '\0';

    printf("Debug: remove_white_spaces input: '%s'\n", source_line);
    printf("Debug: remove_white_spaces output: '%s'\n", dest_line);
}

/* Removes the prefix white spaces from the source_line and saves the result in dest_line. */
void remove_prefix_white_spaces(char *source_line, char *dest_line)
{
    while(isspace((unsigned char)*source_line)) source_line++;
    strcpy(dest_line, source_line);
}

/* Removes the macro indentation from the sourceline and saves the result in dest_line. */
void remove_macro_indentation(char *sourceline, char *dest_line)
{
    strcpy(dest_line, sourceline + FOUR_CHARS_INDENTATION);
}

/* Checks if a string represents an integer. */
int is_integer(const char *str)
{
    if(*str == '-' || *str == '+') str++;
    if(*str == '\0') return FALSE;
    
    while(*str)
    {
        if(!isdigit((unsigned char)*str))
            return FALSE;
        str++;
    }
    
    return TRUE;
}

/* Converts a string to an integer. */
int str_to_int(const char *str)
{
    if(!is_integer(str))
        return -1;  /* or some other error value */
    
    return atoi(str);
}

/* Print to an output file a decimal number in octal base (5 digits). */
void print_file_decimal_to_octal(int decimal, FILE *output_file)
{
    fprintf(output_file, "%05o", decimal & 0x7FFF);  /* Mask to 15 bits */
}

/* Remove a filename suffix. */
void remove_suffix(const char* src_filename, char* dest_filename, const char* suffix)
{
    size_t len = strlen(src_filename);
    size_t suffix_len = strlen(suffix);

    if (len > suffix_len && strcmp(src_filename + len - suffix_len, suffix) == 0) {
        strncpy(dest_filename, src_filename, len - suffix_len);
        dest_filename[len - suffix_len] = '\0';
    } else {
        strcpy(dest_filename, src_filename);
    }
} 
