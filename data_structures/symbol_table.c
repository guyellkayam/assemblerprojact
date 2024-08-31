#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Creates symbol Node */
/* Creates symbol Node */
Symbol * create_symbol(const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address)
{
    Symbol *symbol = (Symbol *)calloc(1, sizeof(Symbol));
    if (symbol == NULL)
    {
        fprintf(stderr, "Allocation failed for a new symbol\n");
        exit(1);
    }

    strncpy(symbol->symbol_name, symbol_name, MAX_LABEL_LENGTH - 1);
    symbol->symbol_name[MAX_LABEL_LENGTH - 1] = '\0';  /* Ensure null-termination */
    symbol->def_line = def_line;
    symbol->address = (address != NULL) ? *address : 0;
    symbol->symbol_opt = symbol_opt;
    symbol->next_symbol = NULL;

    return symbol;
}

Symbol * insert_symbol_to_table(Symbol *symbol, const char *symbol_name, unsigned int def_line, SymbolType symbol_opt, unsigned int *address)
{
    Symbol *current;
    Symbol *existing;
    const char *trimmed_name = symbol_name;

    /* Trim leading whitespace */
    while (*trimmed_name == ' ' || *trimmed_name == '\t')
        trimmed_name++;

    if (trimmed_name == NULL || *trimmed_name == '\0') {
        fprintf(stderr, "Error: Attempting to insert a NULL or empty symbol name\n");
        return symbol;
    }

    /* Check if the symbol already exists */
    existing = get_symbol(symbol, trimmed_name);
    if (existing) {
        /* Update existing symbol if it's not an external symbol */
        if (existing->symbol_opt != symbol_extern_def) {
            existing->address = (address != NULL) ? *address : 0;
            existing->symbol_opt = symbol_opt;
        }
        return symbol;
    }

    /* Create new symbol if it doesn't exist */
    if (symbol == NULL) 
    {
        return create_symbol(trimmed_name, def_line, symbol_opt, address);
    }

    current = symbol;
    while (current->next_symbol != NULL) 
    {
        current = current->next_symbol;
    }

    current->next_symbol = create_symbol(trimmed_name, def_line, symbol_opt, address);
    printf("Inserted symbol: %s, Address: %u, Type: %d\n", current->next_symbol->symbol_name, current->next_symbol->address, current->next_symbol->symbol_opt);
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
            if (symbol->symbol_opt == symbol_extern_def) 
            {
                return ARE_EXTERN;
            }
            else if (symbol->symbol_opt != symbol_extern_def)
            {
                return (symbol->address << ARE_INDENTATION) | ARE_NEW;
            }
        }
         
        symbol = symbol->next_symbol;
    }

    return 0;  /* Return 0 to indicate symbol not found */
}

/* Get "entry" symbols that are used as start points. */
Symbol * get_entry_calls(Symbol *symbol_table, Symbol *entry_calls)
{
    Symbol *temp = symbol_table;

    while (temp != NULL)
    {
        if (temp->symbol_opt == symbol_entry_def || 
            temp->symbol_opt == symbol_local_data || 
            temp->symbol_opt == symbol_local_code)
        {
            Symbol *existing_entry = get_symbol(entry_calls, temp->symbol_name);
            if (existing_entry != NULL)
            {
                printf("Debug: Updating entry symbol %s with address %u\n", temp->symbol_name, temp->address);
                existing_entry->address = temp->address;
            }
            else
            {
                printf("Debug: Adding entry symbol %s with address %u\n", temp->symbol_name, temp->address);
                entry_calls = insert_symbol_to_table(entry_calls, temp->symbol_name, temp->def_line, symbol_entry_def, &(temp->address));
            }
        }
        temp = temp->next_symbol;
    }

    return entry_calls;
}

/* Frees the entire symbol table and all the symbols it contains. */
void free_symbol_table(Symbol *symbol) 
{
    Symbol *temp;
    while (symbol != NULL) 
    {
        temp = symbol;
        symbol = symbol->next_symbol;
        free(temp);
    }
} 
