#ifndef LOGS
#define LOGS

void create_log_file();

void register_copy_CSV(const char *file_name, pid_t pid, double duration);

#endif