#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>


#define COMMENT_PREFIX ';'
#define MAX_LINE_LENGTH 81
#define MAX_FILE_NAME_LENGTH 256
#define MAX_LABEL_LENGTH 31
#define MAX_OPERAND_LENGTH 31
#define FOUR_CHARS_INDENTATION 4
/* #define MACRO_DEF_STR_LENGTH 5 */
#define TRUE 1
#define FALSE 0
#define DOT_AM_SUFFIX ".am"
#define DOT_OB_SUFFIX ".ob"
#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define WORD_SIZE 15
#define MEMORY_SIZE 4096
#define BASE_ADDRESS 100
#define MACRO_DEF_STR_LENGTH 31  /* Define the macro here */

/**
 * @brief Removes all white spaces from the source_line and saves the result in dest_line.
 * 
 * @param source_line The input string with white spaces.
 * @param dest_line The output string without white spaces.
 */
void remove_white_spaces(char *source_line, char *dest_line);

/**
 * @brief Removes the prefix white spaces from the source_line and saves the result in dest_line.
 * 
 * @param source_line The input string with prefix white spaces.
 * @param dest_line The output string without prefix white spaces.
 */
void remove_prefix_white_spaces(char *source_line, char *dest_line);

/**
 * @brief Removes the macro indentation from the sourceline and saves the result in dest_line.
 * The macro indentation is assumed to start at index 4 of sourceline.
 * 
 * @param sourceline The input string with macro indentation.
 * @param dest_line The output string without macro indentation.
 * @note I assume, macro content is written in indentation of 4 chars.
 */
void remove_macro_indentation(char *sourceline, char *dest_line);

/**
 * @brief Checks if a string represents an integer.
 * 
 * @param str The input string to check if it represents an integer.
 * @return TRUE (1) if the str is an integer, otherwise FALSE (0).
 */
int is_integer(const char *str); 

/**
 * @brief Converts a string to an integer.
 * 
 * @param str The input string to convert to an integer.
 * @return The integer value of the string, or -1 if the string is not a valid integer.
 */
int str_to_int(const char *str); 

/**
 * @brief Print to the terminal a decimal number in binary base (15 bits).
 * 
 * @param decimal The decimal number to print in binary base.
 */
void print_terminal_decimal_to_binary(int decimal); 

/**
 * @brief Print to an output file a decimal number in binary base (15 bits).
 * 
 * @param decimal The decimal number to print in binary base.
 * @param output_file The file to print to.
 */
void print_file_decimal_to_binary(int decimal, FILE * output_file);

/**
 * @brief Print to an output file a decimal number in octal base (5 digits).
 * 
 * @param decimal The decimal number to print in octal base.
 * @param output_file The file to print to.
 */
void print_file_decimal_to_octal(int decimal, FILE *output_file);

/**
 * @brief Remove a filename suffix.
 * 
 * @param src_filename The original filename with the suffix.
 * @param dest_filename The new filename without the suffix.
 * @param suffix The suffix to remove.
 */
void remove_suffix(const char* src_filename, char* dest_filename, const char* suffix); 

/**
 * @brief Check if a string is a valid label.
 *
 * @param str The string to check.
 * @return TRUE (1) if the string is a valid label, FALSE (0) otherwise.
 */
int is_valid_label(const char *str);

#endif /* UTILS_H */ 
