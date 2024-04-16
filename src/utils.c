#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The return has to be freed
char *get_new_filename(char *src_filename, char *dest_dir)
{
    // printf("Get new filename function\n");

    char *temp_src_filename_01, *temp_src_filename_02;
    temp_src_filename_01 = strdup(src_filename);
    temp_src_filename_02 = strdup(src_filename);

    char *filename = basename(temp_src_filename_01);
    char *dir = dirname(temp_src_filename_02);

    // printf("filename: %s, dirname: %s\n", filename, dir);

    char *new_filename;

    char dot = '.', slash = '/';

    if (dir[0] == dot || strchr(dir, slash) == NULL)
    {
        new_filename = malloc(sizeof(filename) + sizeof(dest_dir) + 2);
        strcat(new_filename, dest_dir);
        strcat(new_filename, "/");
        strcat(new_filename, filename);
    }
    else
    {
        char *ptr = strchr(dir, slash);
        ptr++;
        new_filename = malloc(sizeof(filename) + sizeof(ptr) + 3);
        strcat(new_filename, dest_dir);
        strcat(new_filename, "/");
        strcat(new_filename, ptr);
        strcat(new_filename, "/");
        strcat(new_filename, filename);
    }

    // printf("After free\n");

    printf("New filename: %s\n", new_filename);

    free(temp_src_filename_01);
    free(temp_src_filename_02);
    temp_src_filename_01 = NULL, temp_src_filename_02 = NULL;

    return new_filename;
}
