#ifndef ASSEMBLER_HELPER_H
#define ASSEMBLER_HELPER_H

#include "../assembler/assembler.h"

/**
 * @brief Set extra word for instruction based on the operand type.
 * 
 * @param analyzed_line The analyzed_line structure containing the operand types and values.
 * @param compiled_line The compiled_line structure to which the extra word will be written.
 * @param num_of_operands Number of operands for the instruction.
 * @param address Pointer to the current address for inserting words.
 */
void set_inst_extra_words(const Analyzed_line *analyzed_line, Compiled_Line *compiled_line, int num_of_operands, unsigned int *address);

/**
 * @brief Generate assembler warning.
 * 
 * @param line_index The line number where the warning occurred.
 * @param message A message describing the warning.
 */
void assembler_warning(int line_index, const char *message);

/**
 * @brief Generate assembler error.
 * 
 * @param line_index The line number where the error occurred.
 * @param message A message describing the error.
 */
void assembler_error(int line_index, const char *message);

#endif /* ASSEMBLER_HELPER_H */ 
