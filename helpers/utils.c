#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Print to the terminal a decimal number in binary base (15 bits). */
void print_terminal_decimal_to_binary(int decimal)
{
    int binary[15];
    int i;
    int index = 0;

    if (decimal == 0)
    {
        printf("Binary: 0\n");
        return;
    }

    if (decimal < 0)
    {
        printf("Binary: ");
        decimal = (1 << 15) + decimal; // Adjust for 15-bit two's complement
    }
    else
    {
        printf("Binary: ");
    }

    while (index < 15)
    {
        binary[index++] = decimal % 2;
        decimal /= 2;
    }

    for (i = 14; i >= 0; i--)
    {
        printf("%d", binary[i]);
    }

    printf("\n");
}

/* Print to an output file a decimal number in binary base (15 bits). */
void print_file_decimal_to_binary(int decimal, FILE * output_file)
{
    int binary[15];
    int i;
    int index = 0;

    if (decimal == 0)
    {
        fprintf(output_file, "Binary: 0\n");
        return;
    }

    if (decimal < 0)
    {
        fprintf(output_file, "Binary: ");
        decimal = (1 << 15) + decimal; // Adjust for 15-bit two's complement
    }
    else
    {
        fprintf(output_file, "Binary: ");
    }

    while (index < 15)
    {
        binary[index++] = decimal % 2;
        decimal /= 2;
    }

    for (i = 14; i >= 0; i--)
    {
        fprintf(output_file, "%d", binary[i]);
    }

    fprintf(output_file, "\n");
}