#include <stdbool.h>

#define MAX_MSG_LEN 4095

#define COPY_FILE 1
#define DONE 2

struct msgbuf
{
    long mtype;
    char mtext[MAX_MSG_LEN];
};

int create_msg_queue();

bool is_msg_queue_empty(int msqid);

void delete_msg_queue(int msqid);

void send_msg(int msqid, long type, char *msg, bool flag);

char *receive_msg(int msqid, long type, bool flag);