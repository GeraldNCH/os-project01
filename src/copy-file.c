#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void copy_file(char *filename, char *dirname)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening the file %s to read the file content\n", filename);
        exit(1);
    }

    printf("After fopen\n");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("file size: %ld\n", file_size);

    char *file_content = malloc(file_size + 1);
    fread(file_content, file_size, 1, file);

    fclose(file);

    printf("After read\n");

    char *temp_filename = basename(filename);

    char *new_filename = malloc(sizeof(temp_filename) + sizeof(dirname) + 1);
    strcat(new_filename, dirname);
    strcat(new_filename, temp_filename);

    printf("New filename: %s\n", new_filename);

    FILE *new_file = fopen(new_filename, "w");
    if (new_file == NULL)
    {
        printf("Error creating the file %s to write the file content\n", new_filename);
        exit(1);
    }
    int result = fwrite(file_content, file_size, 1, new_file);

    fclose(new_file);

    free(file_content);
    free(new_filename);

    return;
}