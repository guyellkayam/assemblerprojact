#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Creates symbol Node */
Symbol * create_symbol(const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address)
{
    Symbol *symbol = (Symbol *)calloc(1, sizeof(Symbol));
    if (symbol == NULL)
    {
        printf("Allocation failed for a new symbol\n");
        exit(0);
    }

    strcpy(symbol->symbol_name, symbol_name);
    symbol->def_line = def_line;
    symbol->address = *address;
    symbol->symbol_opt = symbol_opt;
    symbol->next_symbol = NULL;

    return symbol;
}

/* Inserts a new symbol into the symbol table. */
Symbol * insert_symbol_to_table(Symbol *symbol, const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address)
{
    if (symbol == NULL) 
    {
        return create_symbol(symbol_name, def_line, symbol_opt, address);
    }

    Symbol *current = symbol;
    while (current->next_symbol != NULL) 
    {
        current = current->next_symbol;
    }

    current->next_symbol = create_symbol(symbol_name, def_line, symbol_opt, address);
    return symbol;
}

/* Searches for a symbol with the symbol_name in the symbol table. */
Symbol * get_symbol(Symbol *symbol, const char *symbol_name)
{
    while (symbol != NULL)
    {
        if (strcmp(symbol->symbol_name, symbol_name) == 0) 
            return symbol;
        
        symbol = symbol->next_symbol;
    }

    return NULL;
}

/* Searches for a symbol in the symbol table and returns the symbol's address if the symbol was declared. */
unsigned int get_symbol_def_address(Symbol *symbol, const char *symbol_name)
{
    while (symbol != NULL)
    {
        if (strcmp(symbol->symbol_name, symbol_name) == 0)
        {
            if (symbol->symbol_opt == SYMBOL_EXTERN_DEF) 
            {
                symbol->address = 0;
                symbol->address |= ARE_EXTERN;
                return symbol->address;
            }
            else if (symbol->symbol_opt != SYMBOL_ENTRY_DEF)
            {
                symbol->address = (symbol->address << ARE_INDENTATION) | ARE_NEW;
                return symbol->address;
            }
        }
         
        symbol = symbol->next_symbol;
    }

    return 0;  // Return 0 to indicate symbol not found
}

/* Get "entry" symbols that are used as start points. */
Symbol * get_entry_calls(Symbol *symbol_table, Symbol *entry_calls)
{
    Symbol *temp = symbol_table;

    while (temp != NULL) 
    {
        if (temp->symbol_opt == SYMBOL_ENTRY_CODE || temp->symbol_opt == SYMBOL_ENTRY_DATA) 
        {
            entry_calls = insert_symbol_to_table(entry_calls, temp->symbol_name, temp->def_line, temp->symbol_opt, &(temp->address));
        }

        temp = temp->next_symbol;
    }

    return entry_calls;
}

/* Frees the memory allocated for a given symbol. */
void free_symbol(Symbol *symbol) 
{
    free(symbol);
}

/* Frees the entire symbol table and all the symbols it contains. */
void free_symbol_table(Symbol *symbol) 
{
    while (symbol != NULL) 
    {
        Symbol *temp = symbol;
        symbol = symbol->next_symbol;
        free_symbol(temp);
    }
}
