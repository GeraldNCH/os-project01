#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>

#define POOL_SIZE 10

int main()
{
    char cwd[PATH_MAX_LENGTH];
    printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    // read_directory("test", "test", "hola");

    // create_dir("nuevo");
    // char *copy = "test/files/otro/si.csv";
    // char *new_dir = change_root_name("test/files", "nuevo");
    // printf("%s\n", new_dir);
    // new_dir = dirname(new_dir);
    // create_dir(new_dir);
    // char *new_filename = change_root_name(copy, "nuevo");
    // printf("new filename: %s\n", new_filename);
    // copy_file(copy, new_filename);
    // free(new_filename);

    key_t msqkey = 999;
    int msqid = msgget(msqkey, IPC_CREAT | S_IRUSR | S_IWUSR); // create queue

    struct msgbuf temp;

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
    // if (pid != 0) // parent
    // {
    //     temp.mtype = COPY_FILE;
    //     strcpy(temp.mtext, "nuevo/hola.txt");
    //     msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT);

    //     temp.mtype = COPY_FILE;
    //     strcpy(temp.mtext, "nuevo/mundo.txt");
    //     msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT);
    //     wait(&status);
    //     msgctl(msqid, IPC_RMID, NULL); // delete queue
    //     exit(0);
    // }
    // else
    // {
    //     printf("Process: %d waiting\n", getpid());
    //     int result = msgrcv(msqid, &temp, PATH_MAX_LENGTH, 0, 1);
    //     if (result != -1)
    //     {
    //         printf("Process: %d received: %s\n", getpid(), temp.mtext);
    //     }
    //     exit(0);
    // }
}