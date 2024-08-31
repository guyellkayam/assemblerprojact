#include "assembler/assembler.h"
#include "preprocessor/preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int i;
    char *am_filename;
    FILE *am_file;
    char cwd[1024];
    int result;
    char line[MAX_LINE_LENGTH];
    int line_number;

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    if (argc < 2) {
        printf("Usage: %s <input_file> [<input_file2> ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) 
    {
        printf("Processing argument: %s\n", argv[i]);

        am_filename = process_as_file(argv[i]);

        if (am_filename == NULL) 
        {
            printf("Failed to process the input file: %s\n", argv[i]);
            continue;
        }

        printf("Successfully processed file. Output filename: %s\n", am_filename);

        am_file = fopen(am_filename, "r");
        if (am_file == NULL) 
        {
            perror("Failed to open the processed file");
            printf("Attempted to open file: %s\n", am_filename);
            free(am_filename);
            continue;
        }

        printf("Successfully opened processed file: %s\n", am_filename);

        /* Debug: Print contents of processed file */
        printf("Debug: Contents of processed file %s:\n", am_filename);
        line_number = 1;
        fseek(am_file, 0, SEEK_SET);  /* Reset file pointer to the beginning */
        while (fgets(line, sizeof(line), am_file)) {
            printf("Debug: Line %d: %s", line_number, line);
            line_number++;
        }
        fseek(am_file, 0, SEEK_SET);  /* Reset file pointer to the beginning */
        printf("Debug: End of file contents\n");

        /* Call assembler and check its return value */
        result = assembler(am_file, am_filename);
        if (result != TRUE) {
            printf("Debug: Assembler failed with result: %d\n", result);
        } else {
            printf("Debug: Assembler completed successfully\n");
        }

        fclose(am_file);
        free(am_filename);
    }

    return 0;
} 
