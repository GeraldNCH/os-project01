#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <time.h>

#include "../headers/archives.h"
#include "../headers/msg-queue.h"
#include "../headers/logs.h"
#include "../headers/processes.h"

int main(int argc, char **argv)
{
    int parent_pid = getpid();

    create_log_file();

    if (argc != 3)
    {
        printf("You must pass 3 arguments to the program\n");
        exit(-1);
    }
    char *src_dir = argv[1];
    char *dest_dir = argv[2];

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

    // char cwd[MAX_MSG_LEN];
    // printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    int msqid = create_msg_queue();

    struct process_pool_control processes_control;
    processes_control.available_processes = PROCESS_POOL_SIZE;
    int is_parent = create_process_pool(processes_control.pids);

    printf("pid: %d, state: %d\n", processes_control.pids[0][0], processes_control.pids[0][1]);

    if (is_parent)
    {
        printf("parent entered\n");
        copy_directory(src_path, dest_path, msqid, &processes_control);

        while (!is_msg_queue_empty(msqid) || processes_control.available_processes != PROCESS_POOL_SIZE)
        {
            struct msgbuf temp;
            receive_msg(msqid, &temp, getpid(), DONE, false);
            processes_control.available_processes++;
        }

        delete_process_pool(processes_control.pids);
        delete_msg_queue(msqid);
    }
    else
    {

        while (true)
        {
            printf("child entered\n");
            struct msgbuf temp;
            receive_msg(msqid, &temp, getpid(), -COPY_FILE, false);

            if (temp.action == CHANGE_DIR)
            {
                int result;
                do
                {
                    result = chdir(temp.mtext);
                } while (result != 0);
            }
            else if (temp.action == CREATE_DIR)
            {
                char *temp_path = strdup(temp.mtext);
                char *dir_name = basename(temp_path);

                create_dir(dir_name);

                send_msg(msqid, parent_pid, DONE, getpid(), dir_name, false);
                free(temp_path);
            }
            else
            {
                char *temp_path = strdup(temp.mtext);
                char *filename = basename(temp_path);

                bool result;
                do
                {
                    result = copy_file(temp.mtext, filename);
                } while (!result);

                send_msg(msqid, parent_pid, DONE, getpid(), filename, false);
                free(temp_path);
            }
        }
    }

    exit(0);
}