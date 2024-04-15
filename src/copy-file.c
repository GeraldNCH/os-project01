#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void copy_file(char *filename, char *dirname)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error opening the file %s to read the file content\n", filename);
        exit(1);
    }

    char *temp_filename = basename(filename);

    char *new_filename = malloc(sizeof(temp_filename) + sizeof(dirname));
    strcat(new_filename, dirname);
    strcat(new_filename, temp_filename);

    printf("New filename: %s\n", new_filename);

    FILE *new_file = fopen(new_filename, "wb");
    if (new_file == NULL)
    {
        printf("Error creating the file %s to write the file content\n", new_filename);
        exit(1);
    }

    char buffer[255];

    while (!feof(file))
    {
        fread(buffer, sizeof(buffer), 1, file);
        fwrite(buffer, sizeof(buffer), 1, new_file);
    }

    fclose(file);
    fclose(new_file);

    free(new_filename);

    return;
}