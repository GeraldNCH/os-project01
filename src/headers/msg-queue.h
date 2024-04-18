#ifndef MSG_QUEUE
#define MSG_QUEUE

#include <stdbool.h>

#define MAX_MSG_LEN 4095

#define CHANGE_DIR 1
#define CREATE_DIR 2
#define COPY_FILE 3
#define DONE 4

struct msgbuf
{
    int mtype;
    int action;
    int sender_pid;
    char mtext[MAX_MSG_LEN];
};

int create_msg_queue();

bool is_msg_queue_empty(int msqid);

void delete_msg_queue(int msqid);

void send_msg(int msqid, int type, int action, int sender_pid, char *msg, bool flag);

bool receive_msg(int msqid, struct msgbuf *temp, int type, int action, bool flag);

int len_msg_queue(int msqid);

int get_last_sender(int msqid);

#endif