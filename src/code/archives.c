#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

#include "../headers/archives.h"
#include "../headers/msg-queue.h"

// Create a directory if not exists.
void create_dir(char *path)
{
    struct stat sb;
    if (stat(path, &sb) == -1)
    {
        if (mkdir(path, S_IRWXU) != 0)
        {
            perror("mkdir");
        }
    }
}

// Copy a file in a new destination and creates the destination path if not exists.
bool copy_file(char *src_filepath, char *dest_filepath)
{
    char *temp_str = strdup(dest_filepath);
    char *new_dest_dir = dirname(temp_str);
    printf("new_dest_dir: %s\n", new_dest_dir);
    create_dir(new_dest_dir);
    free(temp_str);

    FILE *file = fopen(src_filepath, "r");
    if (file == NULL)
    {
        perror("fopen src file");
        return false;
    }

    FILE *new_file = fopen(dest_filepath, "w");
    if (new_file == NULL)
    {
        perror("fopen dest file");
        return false;
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

    return true;
}

// Change the first element of the path for the new specified name.
// The returned string has to be freed.
char *change_root_name(char *old_path, char *new_name)
{
    char *temp_src_filename_01, *temp_src_filename_02;
    temp_src_filename_01 = strdup(old_path);
    temp_src_filename_02 = strdup(old_path);

    char *filename = basename(temp_src_filename_01);
    char *dir = dirname(temp_src_filename_02);

    // printf("filename: %s, dirname: %s\n", filename, dir);

    char *new_filename;

    char dot = '.', slash = '/';

    if (dir[0] == dot || strchr(dir, slash) == NULL)
    {
        new_filename = malloc(sizeof(filename) + sizeof(new_name) + 2);
        new_filename[0] = '\0';
        strcat(new_filename, new_name);
        strcat(new_filename, "/");
        strcat(new_filename, filename);
    }
    else
    {
        char *ptr = strchr(dir, slash);
        ptr++;
        new_filename = malloc(sizeof(filename) + sizeof(ptr) + sizeof(new_name) + 3);
        new_filename[0] = '\0';
        strcat(new_filename, new_name);
        strcat(new_filename, "/");
        strcat(new_filename, ptr);
        strcat(new_filename, "/");
        strcat(new_filename, filename);
    }

    // printf("New filename: %s\n", new_filename);

    free(temp_src_filename_01);
    free(temp_src_filename_02);
    temp_src_filename_01 = NULL, temp_src_filename_02 = NULL;

    return new_filename;
}

void copy_directory(char *dir_name, char *current_path, char *dest_dir, int msqid, int *available_processes)
{
    DIR *dirp = opendir(dir_name);
    if (dirp == NULL)
    {
        perror("opendir");
        exit(-1);
    }

    if (chdir(dir_name) != 0) // Change process cwd
    {
        perror("chdir");
        exit(-1);
    }

    struct dirent *entry;
    struct stat sb;

    while ((entry = readdir(dirp)) != NULL)
    {
        // while ((*available_processes) == 0) // Wait for a process to be available to continue
        // {
        //     struct msgbuf temp;
        //     receive_msg(msqid, &temp, DONE, false);
        //     (*available_processes)++;
        // }

        if (stat(entry->d_name, &sb) != 0)
        {
            perror("stat");
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        if (S_ISDIR(sb.st_mode) != 0) // Is a directory
        {
            char path[MAX_MSG_LEN];
            snprintf(path, sizeof(path), "%s/%s", current_path, entry->d_name);

            send_msg(msqid, CREATE_DIR, path, false);
            (*available_processes)--;

            copy_directory(entry->d_name, path, dest_dir, msqid, available_processes);

            if (chdir("..") != 0) // Return process to previous path
            {
                perror("chdir");
                exit(-1);
            }
        }
        else // Is a file
        {
            char filepath[MAX_MSG_LEN];
            snprintf(filepath, sizeof(filepath), "%s/%s", current_path, entry->d_name);
            // printf("Filepath: %s\n", filepath);

            send_msg(msqid, COPY_FILE, filepath, false);
            (*available_processes)--;
        }
    }
    closedir(dirp);
}