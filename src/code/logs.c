#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "../headers/logs.h"

#define LOGFILE "logfile.csv"

// Function to create the log file (if it does not exist)
void create_log_file() 
{
    FILE *file;
    file = fopen(LOGFILE, "w");
    fprintf(file, "File Name,Process ID,Duration\n");
    fclose(file);
}

// Function to register copy file data in CSV file
void register_copy_CSV(const char *file_name, pid_t pid, double duration) 
{
    FILE *file;
    file = fopen(LOGFILE, "a");
    
    if (file == NULL) {
        perror("Error opening file");
        exit(-1);
    }
    
    fprintf(file, "%s,%d,%f\n", file_name, pid, duration);
    
    fclose(file);
}
