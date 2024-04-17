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
    free(temp_str);
    create_dir(new_dest_dir);

    FILE *file = fopen(src_filepath, "r");
    if (file == NULL)
    {
        perror("fopen");
        return false;
    }

    FILE *new_file = fopen(dest_filepath, "w");
    if (new_file == NULL)
    {
        perror("fopen");
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

void read_directory(char *dir_name, char *parent_dir, char *dest_dir, int msqid, int *actions_count)
{
    DIR *dirp = opendir(dir_name);
    if (dirp == NULL)
    {
        printf("Cannot open the directory\n");
        return;
    }

    int result = chdir(dir_name);
    if (result != 0)
    {
        perror("Error");
        return;
    }

    struct dirent *entry;
    struct stat sb;

    printf("msqid: %d\n", msqid);

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
            snprintf(path, sizeof(path), "%s/%s", parent_dir, entry->d_name);

            struct msgbuf temp;
            temp.mtype = CREATE_DIR;
            strcpy(temp.mtext, path);

            printf("Queue full: %d\n", is_queue_full(msqid));

            if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT) != 0)
            {
                perror("msgsnd");
                exit(-1);
            }

            (*actions_count)++;

            read_directory(entry->d_name, path, dest_dir, msqid, actions_count);
            chdir("..");
        }
        else // Is a file
        {
            char filepath[PATH_MAX_LENGTH];
            snprintf(filepath, sizeof(filepath), "%s/%s", parent_dir, entry->d_name);
            // printf("Filepath: %s\n", filepath);

            struct msgbuf temp;
            temp.mtype = COPY_FILE;
            strcpy(temp.mtext, filepath);

            printf("Queue full: %d\n", is_queue_full(msqid));

            if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT) != 0)
            {
                perror("msgsnd");
                exit(-1);
            }

            (*actions_count)++;
        }
    }
    closedir(dirp);
}