#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/libTeer.h"

char *read_single_record(FILE *tarball)
{
    char *buffer = malloc(LIBTEER_RECORD_LENGTH * sizeof(char));
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory!\n");
        return NULL;
    }
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        buffer[i] = fgetc(tarball);
    }
    return buffer;
}

void print_record(const char *record)
{
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        printf("%c", record[i]);
    }
    printf("\n");
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        printf("%02X", (unsigned int)record[i]);

        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}
