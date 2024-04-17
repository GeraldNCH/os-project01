#include <stdbool.h>

bool create_dir(char *relative_path);

bool copy_file(char *filename, char *new_filename);

char *change_root_name(char *src_filename, char *dest_dir);

void read_directory(char *dir_name, char *parent_dir, char *dest_dir, int msqid, int *actions_count);