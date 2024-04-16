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
#include "utils.h"

#define POOL_SIZE 2

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("You must pass 3 arguments to the program");
        exit(-1);
    }
    char *src_dir = argv[1];
    char *dest_dir = argv[2];

    printf("src dir: %s, dest dir: %s\n", src_dir, dest_dir);

    create_dir(dest_dir);

    char cwd[PATH_MAX_LENGTH];
    printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    key_t msqkey = 999;
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | S_IRUSR | S_IWUSR); // create queue
    if (msqid == -1)
    {
        perror("mssget");
        exit(-1);
    }
    printf("msqid: %d\n", msqid);

    long pids[POOL_SIZE];
    long pid;
    int status;
    for (int i = 0; i < POOL_SIZE; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            break;
        }
        // printf("pid: %ld\n", pid);
        pids[i] = pid;
    }

    if (pid == 0) // child process
    {
        while (1)
        {
            struct msgbuf temp;
            if (msgrcv(msqid, &temp, PATH_MAX_LENGTH, -2, 0) == -1)
            {
                perror("msgrcv");
                exit(-1);
            }

            if (temp.mtype == CREATE_DIR)
            {
                char *new_path = change_root_name(temp.mtext, dest_dir);

                bool result;
                do
                {
                    result = create_dir(new_path);
                } while (!result);

                printf("CREATE_DIR src: %s, dest:%s\n", temp.mtext, new_path);

                free(new_path);

                struct msgbuf temp;
                temp.mtype = DONE;
                strcpy(temp.mtext, "Directory created");

                if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT) == -1)
                {
                    perror("msgsnd");
                    exit(-1);
                }
            }
            else
            {
                char *new_filename = change_root_name(temp.mtext, dest_dir);

                bool result;
                do
                {
                    result = copy_file(temp.mtext, new_filename);
                } while (!result);

                printf("COPY_FILE src: %s, dest: %s\n", temp.mtext, new_filename);

                free(new_filename);

                struct msgbuf temp;
                temp.mtype = DONE;
                strcpy(temp.mtext, "File copied");

                if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT) == -1)
                {
                    perror("msgsnd");
                    exit(-1);
                }
            }
        }
    }
    else // parent process
    {
        struct msqid_ds queue_stats;

        int actions_count = 0;
        read_directory(src_dir, src_dir, dest_dir, msqid, &actions_count);

        printf("actions count: %d\n", actions_count);

        for (int i = 0; i < actions_count; i++)
        {
            printf("Parent waiting for childs %d\n", i);
            struct msgbuf temp;
            msgrcv(msqid, &temp, PATH_MAX_LENGTH, DONE, 0);
            msgctl(msqid, IPC_STAT, &queue_stats);
            printf("messages in queue: %ld\n", queue_stats.msg_qnum);
        }

        // Check if queue is empty

        msgctl(msqid, IPC_STAT, &queue_stats);
        if (queue_stats.msg_qnum != 0)
        {
            printf("The queue is not empty");
        }

        // Kill childs
        for (int i = 0; i < POOL_SIZE; i++)
        {
            kill(pids[i], SIGKILL);
        }

        msgctl(msqid, IPC_RMID, NULL); // delete the queue
        exit(0);
    }
}