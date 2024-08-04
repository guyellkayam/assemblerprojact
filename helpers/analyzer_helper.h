#ifndef ANALYZER_HELPER_H
#define ANALYZER_HELPER_H

#include "utils.h"
#include "../analyzer/analyzer.h"

/**
 * @brief Remove the str quotations to save it as is.
 *
 * @param str_content The string content with quotations.
 * @return A new string with the quotations removed.
 */
char * remove_str_quotations(const char *str_content);

/**
 * @brief Split two instruction's operands
 *
 * @param inst_content Two operands separated by a comma.
 * @param first_operand Pointer to store the first operand.
 * @param second_operand Pointer to store the second operand.
 */
void split_operands(const char *inst_content, char *first_operand, char *second_operand);

/**
 * @brief Get the number of a given register.
 *
 * @param reg The register in 'rX' format.
 * @return The number of the register. 
 */
int get_reg_num(const char *reg);

/**
 * @brief Checks if there is an error that has occurred until now regarding the current line.
 *
 * @param analyzed_line Pointer to the 'analyzed_line' object.
 * @return TRUE (1) if valid, otherwise returns FALSE (0). 
 */
int is_valid_analyzed_line(const Analyzed_line *analyzed_line);

#endif /* ANALYZER_HELPER_H */
