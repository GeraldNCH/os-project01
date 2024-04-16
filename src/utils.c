#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include "utils.h"

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
    content = NULL;

    return;
}

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

    printf("New filename: %s\n", new_filename);

    free(temp_src_filename_01);
    free(temp_src_filename_02);
    temp_src_filename_01 = NULL, temp_src_filename_02 = NULL;

    return new_filename;
}

struct node *read_directory(char *directory_name, char *parent_directory, struct node *head)
{
    DIR *dirp = opendir(directory_name);
    if (dirp == NULL)
    {
        printf("Cannot open the directory\n");
        return NULL;
    }

    int result = chdir(directory_name);
    if (result != 0)
    {
        perror("Error");
        return NULL;
    }

    struct dirent *entry;
    struct stat sb;

    while ((entry = readdir(dirp)) != NULL)
    {
        if (stat(entry->d_name, &sb) == -1)
        {
            perror("Error");
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        if (S_ISDIR(sb.st_mode) != 0) // Is a directory
        {
            char path[PATH_MAX_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", directory_name, entry->d_name);
            // printf("%s\n", path);
            head = read_directory(entry->d_name, path, head);
            chdir("..");
        }
        else // Is a file
        {
            char filename[PATH_MAX_LENGTH];
            snprintf(filename, sizeof(filename), "%s/%s", parent_directory, entry->d_name);
            head = push_node(head, filename);
        }
    }
    closedir(dirp);
}