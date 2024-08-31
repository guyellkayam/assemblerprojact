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
 * @brief Create a new symbol node.
 * 
 * @param symbol_name The name of the Symbol.
 * @param def_line The definition line of the Symbol.
 * @param symbol_opt The type of the symbol.
 * @param address The address of the label.
 * @return Pointer to the newly created Symbol Node.
 */
Symbol *create_symbol(const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address);

/**
 * @brief Inserts a new symbol into the symbol table.
 * 
 * @param symbol The symbol table.
 * @param symbol_name The name of the Symbol.
 * @param def_line The definition line of the Symbol.
 * @param symbol_opt The type of the symbol.
 * @param address The address of the label.
 * @return Pointer to the updated Symbol Table.
 */
Symbol *insert_symbol_to_table(Symbol *symbol, const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address);

/**
 * @brief Searches for a symbol with the given symbol_name in the symbol table.
 * 
 * @param symbol The symbol table.
 * @param symbol_name The name of the Symbol.
 * @return Pointer to the required Symbol or NULL if it doesn't exist.
 */
Symbol *get_symbol(Symbol *symbol, const char *symbol_name);

/**
 * @brief Searches for a symbol in the symbol table and returns the symbol's address if the symbol was declared.
 * 
 * @param symbol The symbol table.
 * @param symbol_name The name of the Symbol.
 * @return Address of the symbol if declared; otherwise returns 0.
 */
unsigned int get_symbol_def_address(Symbol *symbol, const char *symbol_name);

/**
 * @brief Get "entry" symbols that are used as a starting point.
 * 
 * @param symbol_table The symbol table.
 * @param entry_calls The entry calls table.
 * @return Pointer to the entry calls table.
 */
Symbol *get_entry_calls(Symbol *symbol_table, Symbol *entry_calls);

/**
 * @brief Frees the memory allocated for a given symbol.
 * 
 * @param symbol A symbol Node.
 */
void free_symbol(Symbol *symbol); 

/**
 * @brief Frees the entire symbol table and all the symbols it contains.
 * 
 * @param symbol The symbol table.
 */
void free_symbol_table(Symbol *symbol); 

#endif /* SYMBOL_TABLE_H */
