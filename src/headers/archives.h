#ifndef ARCHIVES
#define ARCHIVES

#include <stdbool.h>

#include "../headers/processes.h"

void create_dir(char *path);

bool copy_file(char *src_filepath, char *dest_filepath);

void copy_directory(char *src_dir, char *dest_dir, int msqid, struct process_pool_control *processes_control, char *log_file_path);

#endif