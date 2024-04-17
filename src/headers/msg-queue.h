#include <stdbool.h>

#define MAX_MSG_LEN 4095

#define CREATE_DIR 1
#define COPY_FILE 2
#define DONE 3

struct msgbuf
{
    long mtype;
    char mtext[MAX_MSG_LEN];
};

int create_msg_queue();

bool is_msg_queue_empty(int msqid);

void delete_msg_queue(int msqid);

void send_msg(int msqid, long type, char *msg, bool flag);

bool receive_msg(int msqid, struct msgbuf *temp, long type, bool flag);

int len_msg_queue(int msqid);

pid_t get_last_sender(int msqid);