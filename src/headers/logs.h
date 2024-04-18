#ifndef LOGS
#define LOGS

void create_log_file(char *filepath);

void add_entry_log_file(const char *filepath, char *filename, int pid, double duration);

#endif