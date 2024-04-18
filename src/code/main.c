#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "../headers/archives.h"
#include "../headers/msg-queue.h"
#include "../headers/logs.h"

#define PROCESS_POOL_SIZE 2

// Create the process pool.
// Returns 0 if it is a child process.
// Returns 1 if it is the parent process.
int create_process_pool(pid_t pids[PROCESS_POOL_SIZE])
{
    pid_t pid;
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            return 0;
        }
        pids[i] = pid;
    }
    return 1;
}

// Delete the process pool.
void delete_process_pool(pid_t pids[PROCESS_POOL_SIZE])
{
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        if (kill(pids[i], SIGKILL) != 0)
        {
            perror("kill");
            exit(-1);
        }
    }
}

int main(int argc, char **argv)
{
    create_log_file(); 

    if (argc != 3)
    {
        printf("You must pass 3 arguments to the program\n");
        exit(-1);
    }
    char *src_dir = argv[1];
    char *dest_dir = argv[2];

    printf("src dir: %s, dest dir: %s\n", src_dir, dest_dir);

    create_dir(dest_dir);

    // char cwd[MAX_MSG_LEN];
    // printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    int msqid = create_msg_queue();

    pid_t pids[PROCESS_POOL_SIZE];
    int is_parent = create_process_pool(pids);

    if (is_parent)
    {
        int available_processes = PROCESS_POOL_SIZE;
        copy_directory(src_dir, src_dir, dest_dir, msqid, &available_processes);

        while (!is_msg_queue_empty(msqid) || available_processes != 2)
        {
            struct msgbuf temp;
            receive_msg(msqid, &temp, DONE, false);
            available_processes++;
        }

        delete_process_pool(pids);
        delete_msg_queue(msqid);
    }
    else
    {
        while (true)
        {
            struct msgbuf temp;
            receive_msg(msqid, &temp, -COPY_FILE, false);

            if (temp.mtype == CREATE_DIR)
            {
                char *new_path = change_root_name(temp.mtext, dest_dir);
                create_dir(new_path);
                free(new_path);
            }
            else
            {
                char *new_filepath = change_root_name(temp.mtext, dest_dir);
                // printf("filepath: %s, new_filepath: %s\n", temp.mtext, new_filepath);
                bool result;
                do
                {
                    result = copy_file(temp.mtext, new_filepath);
                } while (!result);
                free(new_filepath);
            }
            send_msg(msqid, DONE, "Action completed", false);
        }
    }

    exit(0);
}