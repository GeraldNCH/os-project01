#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "../headers/logs.h"

// Function to create the log file. Overwrite if exist. 
void create_log_file(char *filepath)
{
    FILE *file;
    file = fopen(filepath, "w");
    if (file == NULL)
    {
        perror("fopen logfile");
        exit(-1);
    }

    fprintf(file, "File Name,Process ID,Duration\n");
    fclose(file);
}

// Function to register copy file data in CSV file
void add_entry_log_file(const char *filepath, char *filename, int pid, double duration)
{
    FILE *file;
    file = fopen(filepath, "a");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(-1);
    }

    fprintf(file, "%s,%d,%lf\n", filename, pid, duration);

    fclose(file);
}
