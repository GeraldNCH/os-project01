#ifndef PROCESSES
#define PROCESSES

#define PROCESS_POOL_SIZE 4

// Struct to control the process pool.
struct process_pool_control
{
    int pids[PROCESS_POOL_SIZE][2];
    int available_processes;
};

int create_process_pool(int pids[PROCESS_POOL_SIZE][2]);

void delete_process_pool(int pids[PROCESS_POOL_SIZE][2]);

int get_free_process_pid(int pids[PROCESS_POOL_SIZE][2]);

void set_process_state(int pids[PROCESS_POOL_SIZE][2], int pid, int state);

#endif