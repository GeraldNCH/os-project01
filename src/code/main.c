#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <sys/time.h>

#include "../headers/archives.h"
#include "../headers/msg-queue.h"
#include "../headers/logs.h"
#include "../headers/processes.h"

int main(int argc, char **argv)
{
    int parent_pid = getpid();

    if (argc != 4)
    {
        printf("You must pass 4 arguments to the program\n");
        exit(-1);
    }
    char *src_dir = argv[1];
    char *dest_dir = argv[2];
    char *log_file_name = argv[3];

    // printf("src dir: %s, dest dir: %s\n", src_dir, dest_dir);

    // Set paths
    char original_path[MAX_MSG_LEN], src_path[MAX_MSG_LEN], dest_path[MAX_MSG_LEN];
    getcwd(original_path, sizeof(original_path));
    if (chdir(src_dir) != 0)
    {
        printf("The source directory doesn't exist\n");
        exit(-1);
    }
    else
    {
        getcwd(src_path, sizeof(src_path));
    }
    if (chdir(original_path) != 0)
    {
        perror("chdir");
        exit(-1);
    }
    if (chdir(dest_dir) != 0)
    {
        printf("The destination directory doesn't exist\n");
        exit(-1);
    }
    else
    {
        getcwd(dest_path, sizeof(dest_path));
    }
    if (chdir(original_path) != 0)
    {
        perror("chdir");
        exit(-1);
    }

    printf("ORIGINAL PATH: %s, SRC_PATH: %s, DEST_PATH: %s\n", original_path, src_path, dest_path);

    // Initialize log file
    char log_file_path[MAX_MSG_LEN];
    snprintf(log_file_path, sizeof(log_file_path) + 13, "%s/%s", original_path, log_file_name);
    create_log_file(log_file_path);

    int msqid = create_msg_queue();

    struct process_pool_control processes_control;
    processes_control.available_processes = PROCESS_POOL_SIZE;
    int is_parent = create_process_pool(processes_control.pids);

    if (is_parent)
    {
        copy_directory(src_path, dest_path, msqid, &processes_control, log_file_path);

        while (!is_msg_queue_empty(msqid) || processes_control.available_processes != PROCESS_POOL_SIZE)
        {
            struct msgbuf temp;
            receive_msg(msqid, &temp, FILE_COPIED, false);
            processes_control.available_processes++;
            add_entry_log_file(log_file_path, temp.mtext, temp.sender_pid, temp.copy_duration);
        }

        delete_process_pool(processes_control.pids);
        delete_msg_queue(msqid);
    }
    else
    {
        while (true)
        {
            // Change directory block
            struct msgbuf temp_01;
            receive_msg(msqid, &temp_01, getpid(), false);
            if (chdir(temp_01.mtext) != 0)
            {
                perror("chdir");
                exit(-1);
            }
            send_msg(msqid, DIR_CHANGED, "Directory changed", DIR_CHANGED, getpid(), 0, false);

            // Copy file block
            struct msgbuf temp_02;
            receive_msg(msqid, &temp_02, getpid(), false);
            struct timeval start_time, end_time;
            gettimeofday(&start_time, NULL);
            char *temp_path = strdup(temp_02.mtext);
            char *filename = basename(temp_path);
            copy_file(temp_02.mtext, filename);
            gettimeofday(&end_time, NULL);
            double duration = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
            duration += (end_time.tv_usec - start_time.tv_usec) / 1000.0;
            send_msg(msqid, FILE_COPIED, filename, FILE_COPIED, getpid(), duration, false);
            free(temp_path);
        }
    }
    exit(0);
}