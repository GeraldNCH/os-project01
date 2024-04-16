#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void copy_file(char *filename, char *new_filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening the file %s to read the file content\n", filename);
        exit(1);
    }

    FILE *new_file = fopen(new_filename, "w");
    if (new_file == NULL)
    {
        printf("Error creating the file %s to write the file content\n", new_filename);
        free(new_filename);
        exit(1);
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *content = malloc(file_size);

    fread(content, file_size, 1, file);
    fwrite(content, file_size, 1, new_file);

    fclose(file);
    fclose(new_file);

    free(content);

    return;
}