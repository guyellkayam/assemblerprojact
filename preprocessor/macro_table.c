#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_table.h"

/* Creates a new Macro struct and initializes it with the macro_name. */
Macro *create_macro(const char macro_name[MAX_LINE_LENGTH]) {
    Macro *macro;
    
    macro = (Macro *)calloc(1, sizeof(Macro));
    if (macro == NULL) {
        perror("Allocation failed for a new macro");
        exit(EXIT_FAILURE);
    }

    strncpy(macro->name, macro_name, MAX_LINE_LENGTH - 1);
    macro->name[MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null termination */
    macro->lines = NULL; /* Initially, no lines */
    macro->num_of_lines = 0;
    macro->next_macro = NULL;
    return macro;
}

/* Inserts a new macro with the given macro name into the macro table. */
Macro *insert_macro_to_table(Macro *macro, const char *name) {
    Macro *current;

    if (macro == NULL) {
        return create_macro(name);
    }

    current = macro;
    while (current->next_macro != NULL) {
        current = current->next_macro;
    }
    current->next_macro = create_macro(name);
    return macro;
}

/* Searches for a macro with the given name in the macro table. */
Macro *get_macro(Macro *macro, const char *macro_name) {
    while (macro != NULL) {
        if (strcmp(macro->name, macro_name) == 0) {
            return macro;
        }
        macro = macro->next_macro;
    }
    return NULL;
}

/* Inserts a new line into the lines array of the given macro. */
void insert_macro_line(Macro *macro, const char *line) {
    char **new_lines;

    new_lines = realloc(macro->lines, (macro->num_of_lines + 1) * sizeof(char *));
    if (new_lines == NULL) {
        perror("Memory allocation failed for the macro's lines");
        exit(EXIT_FAILURE);
    }
    macro->lines = new_lines;

    macro->lines[macro->num_of_lines] = calloc(MAX_LINE_LENGTH, sizeof(char));
    if (macro->lines[macro->num_of_lines] == NULL) {
        perror("Memory allocation failed for a new macro's line");
        exit(EXIT_FAILURE);
    }

    strncpy(macro->lines[macro->num_of_lines], line, MAX_LINE_LENGTH - 1);
    macro->lines[macro->num_of_lines][MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null termination */
    macro->num_of_lines += 1;
}

/* Frees the memory allocated for the lines array of a given macro. */
void free_macro_lines(Macro *macro) {
    int i;

    for (i = 0; i < macro->num_of_lines; i++) {
        free(macro->lines[i]);
    }
    free(macro->lines);
}

/* Frees the memory allocated for the given macro and its associated lines. */
void free_macro(Macro *macro) {
    free_macro_lines(macro);
    free(macro);

}

/* Frees the entire macro table and all the macros it contains. */
void free_macro_table(Macro *macro) 
{
    Macro *temp;

    while (macro != NULL) 
    {
        temp = macro;
        macro = macro->next_macro;
        free_macro(temp);
    }

}
