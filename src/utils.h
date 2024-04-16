#define PATH_MAX_LENGTH 4095

#define COPY_FILE 1
#define CREATE_DIR 2
#define DONE 3

struct msgbuf
{
    long mtype; // dest_pid
    char mtext[PATH_MAX_LENGTH];
};

void create_dir(char *relative_path);

void copy_file(char *filename, char *new_filename);

char *change_root_name(char *src_filename, char *dest_dir);

void read_directory(char *dir_name, char *parent_dir, char *dest_dir);