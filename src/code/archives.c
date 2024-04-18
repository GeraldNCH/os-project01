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
#include <time.h>

#include "../headers/archives.h"
#include "../headers/msg-queue.h"
#include "../headers/logs.h"

#define BUFFER_SIZE 4096

// Create a directory if not exists.
void create_dir(char *path)
{
    printf("FUNCTION create_dir\n");

    struct stat sb;
    if (stat(path, &sb) == -1)
    {
        if (mkdir(path, S_IRWXU) != 0)
        {
            perror("mkdir");
        }
    }
}

// Compare 2 directories.
// Return true if they are the same and false if they are different.
bool compare_directory(char *path)
{
    char cwd[MAX_MSG_LEN];
    getcwd(cwd, sizeof(cwd));

    return (strcmp(cwd, path) == 0) ? true : false;
}

// Copies a file in the specified path.
bool copy_file(char *src_filepath, char *dest_filepath)
{
    FILE *src_file = fopen(src_filepath, "rb");
    if (src_file == NULL)
    {
        perror("fopen src file");
        return false;
    }

    FILE *dest_file = fopen(dest_filepath, "wb");
    if (dest_file == NULL)
    {
        perror("fopen dest file");
        fclose(src_file);
        return false;
    }

    // Buffer for copying data
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0)
    {
        if (fwrite(buffer, 1, bytes_read, dest_file) != bytes_read)
        {
            perror("fwrite");
            fclose(src_file);
            fclose(dest_file);
            return false;
        }
    }

    fclose(src_file);
    fclose(dest_file);

    return true;
}

// Change the first element of the path for the new specified name.
// The returned string has to be freed.
// char *change_root_name(char *old_path, char *new_name)
// {
//     printf("FUNCTION change_root_name ARGS old_path: %s, new_name: %s\n", old_path, new_name);

//     char *temp_src_filename_01, *temp_src_filename_02;
//     temp_src_filename_01 = strdup(old_path);
//     temp_src_filename_02 = strdup(old_path);

//     char *filename = basename(temp_src_filename_01);
//     char *dir = dirname(temp_src_filename_02);

//     char *new_filename;

//     char dot = '.', slash = '/';

//     if (dir[0] == dot || strchr(dir, slash) == NULL)
//     {
//         size_t filename_len = strlen(filename);
//         new_filename = malloc(filename_len + strlen(new_name) + 2); // Add 2 for '/' and '\0'
//         new_filename[0] = '\0';
//         strcat(new_filename, new_name);
//         strcat(new_filename, "/");
//         strcat(new_filename, filename);
//     }
//     else
//     {
//         char *ptr = strchr(dir, slash);
//         ptr++;
//         size_t filename_len = strlen(filename);
//         size_t ptr_len = strlen(ptr);
//         new_filename = malloc(filename_len + ptr_len + strlen(new_name) + 3); // Add 3 for '/', '\0', and additional safety
//         new_filename[0] = '\0';
//         strcat(new_filename, new_name);
//         strcat(new_filename, "/");
//         strcat(new_filename, ptr);
//         strcat(new_filename, "/");
//         strcat(new_filename, filename);
//     }

//     free(temp_src_filename_01);
//     free(temp_src_filename_02);
//     temp_src_filename_01 = NULL, temp_src_filename_02 = NULL;

//     return new_filename;
// }

// Copy the contents of a source directory to a destination directory.
void copy_directory(char *src_dir, char *dest_dir, int msqid, struct process_pool_control *processes_control)
{
    printf("FUNCTION copy_directory ARGS src_dir: %s, dest_dir: %s, msqid: %d, processes_control.available_processes: %d\n", src_dir, dest_dir, msqid, processes_control->available_processes);

    DIR *dirp = opendir(src_dir);
    if (dirp == NULL)
    {
        perror("opendir");
        exit(-1);
    }

    if (chdir(src_dir) != 0) // Change process cwd
    {
        perror("chdir");
        exit(-1);
    }

    struct dirent *entry;
    struct stat sb;

    while ((entry = readdir(dirp)) != NULL)
    {
        if (get_last_sender(msqid) != getpid())
        {
            int queue_len = len_msg_queue(msqid);
            for (int i = 0; i < queue_len; i++)
            {
                struct msgbuf temp;
                bool result = receive_msg(msqid, &temp, getpid(), true);
                if (result)
                {
                    set_process_state(processes_control->pids, temp.sender_pid, 1);
                    processes_control->available_processes++;
                }
            }
        }

        while (processes_control->available_processes == 0) // Wait for a process to be available to continue
        {
            struct msgbuf temp;
            receive_msg(msqid, &temp, getpid(), false);
            set_process_state(processes_control->pids, temp.sender_pid, 1);
            processes_control->available_processes++;
        }

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
            char new_src_dir[MAX_MSG_LEN], new_dest_dir[MAX_MSG_LEN];
            snprintf(new_src_dir, sizeof(new_src_dir), "%s/%s", src_dir, entry->d_name);
            snprintf(new_dest_dir, sizeof(new_dest_dir), "%s/%s", dest_dir, entry->d_name);

            create_dir(new_dest_dir);

            copy_directory(new_src_dir, new_dest_dir, msqid, processes_control);

            if (chdir("..") != 0) // Return process to previous path
            {
                perror("chdir");
                exit(-1);
            }
        }
        else // Is a file
        {
            char filepath[MAX_MSG_LEN];
            snprintf(filepath, sizeof(filepath), "%s/%s", src_dir, entry->d_name);

            int pid = get_free_process_pid(processes_control->pids);

            send_msg(msqid, pid, dest_dir, CHANGE_DIR, getpid(), 0, false);
            struct msgbuf temp;
            receive_msg(msqid, &temp, getpid(), false);

            send_msg(msqid, pid, filepath, COPY_FILE, getpid(), 0, false);

            set_process_state(processes_control->pids, pid, 0);
            processes_control->available_processes--;
        }
    }
    closedir(dirp);
}