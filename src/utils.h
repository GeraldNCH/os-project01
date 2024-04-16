#include "files-list.h"

#define PATH_MAX_LENGTH 4095
void copy_file(char *filename, char *new_filename01);
char *get_new_filename(char *src_filename, char *dest_dir);
struct node *read_directory(char *directory_name, char *parent_directory, struct node *head);