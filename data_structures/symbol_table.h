#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../helpers/utils.h"

#define ARE_INDENTATION 2
#define ARE_EXTERN 1 /* 0B01 */
#define ARE_NEW 2 /* 0B10 */

/* Define the SymbolType enumeration */
typedef enum {
    symbol_entry_def,
    symbol_entry_data,
    symbol_entry_code,
    symbol_local_data,
    symbol_local_code,
    symbol_extern_def
} SymbolType;

typedef struct Symbol
{
    char symbol_name[MAX_LABEL_LENGTH];
    unsigned int def_line;
    unsigned int address;
    SymbolType symbol_opt; /* Use SymbolType here */
    struct Symbol *next_symbol;
} Symbol;

/**
 * @brief Creates a new symbol with the given parameters.
 *
 * @param symbol_name The name of the symbol.
 * @param def_line The line number where the symbol is defined.
 * @param symbol_opt The type of the symbol.
 * @param address Pointer to the address of the symbol.
 * @return Pointer to the newly created Symbol.
 */
Symbol *create_symbol(const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address);

Symbol *insert_symbol_to_table(Symbol *symbol, const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address);
Symbol *get_symbol(Symbol *symbol, const char *symbol_name);
unsigned int get_symbol_def_address(Symbol *symbol, const char *symbol_name);
Symbol *get_entry_calls(Symbol *symbol_table, Symbol *entry_calls);
void free_symbol(Symbol *symbol); 
void free_symbol_table(Symbol *symbol); 

#endif /* SYMBOL_TABLE_H */ 
