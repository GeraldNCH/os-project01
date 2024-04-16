#include <stdbool.h>

#define PATH_MAX_LENGTH 4095

#define CREATE_DIR 1
#define COPY_FILE 2
#define DONE 3

struct msgbuf
{
    long mtype; // dest_pid
    char mtext[PATH_MAX_LENGTH];
};

bool create_dir(char *relative_path);

bool copy_file(char *filename, char *new_filename);

char *change_root_name(char *src_filename, char *dest_dir);

void read_directory(char *dir_name, char *parent_dir, char *dest_dir, int msqid, int *actions_count);

bool is_queue_full(int msqid);