#include "assembler/assembler.h"
#include "preprocessor/preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) 
    {
        char *am_filename;

        am_filename = process_as_file(argv[i]);

        if (am_filename == NULL) 
        {
            printf("Failed to process the input file: %s\n", argv[i]);
            continue; // Skip to the next file
        }

        FILE *am_file = fopen(am_filename, "r");
        if (am_file == NULL) 
        {
            printf("Failed to open the file: %s\n", am_filename);
            free(am_filename);
            continue; // Skip to the next file
        }

        // Process the .am file with the assembler
        assembler(am_file, am_filename);

        fclose(am_file);
        free(am_filename);
    }

    return 0;
}