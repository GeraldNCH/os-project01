#include <stdbool.h>

void create_dir(char *path);

bool copy_file(char *src_filepath, char *dest_filepath);

char *change_root_name(char *old_path, char *new_name);

void copy_directory(char *dir_name, char *current_path, char *dest_dir, int msqid, int *available_processes);