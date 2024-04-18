#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "../headers/processes.h"

// Create the process pool.
// Returns 0 if it is a child process.
// Returns 1 if it is the parent process.
int create_process_pool(int pids[PROCESS_POOL_SIZE][2])
{
    int pid;
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            return 0;
        }
        pids[i][0] = pid;
        pids[i][1] = 1;
    }
    return 1;
}

// Delete the process pool.
void delete_process_pool(int pids[PROCESS_POOL_SIZE][2])
{
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        if (kill(pids[i][0], SIGKILL) != 0)
        {
            perror("kill");
            exit(-1);
        }
    }
}

// Get the pid of a free process.
int get_free_process_pid(int pids[PROCESS_POOL_SIZE][2])
{
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        if (pids[i][1] == 1)
        {
            return pids[i][0];
        }
    }
}

// Set the process state.
// State 1 indicates that the process is free and state 0 indicates that the process is busy.
void set_process_state(int pids[PROCESS_POOL_SIZE][2], int pid, int state)
{
    for (int i = 0; i < PROCESS_POOL_SIZE; i++)
    {
        if (pids[i][0] == pid)
        {
            pids[i][1] = state;
            return;
        }
    }
}