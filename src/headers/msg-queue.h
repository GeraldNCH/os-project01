#ifndef MSG_QUEUE
#define MSG_QUEUE

#include <stdbool.h>

#define MAX_MSG_LEN 4095

#define CHANGE_DIR 1
#define COPY_FILE 2
#define DONE 3

struct msgbuf
{
    long mtype;
    char mtext[MAX_MSG_LEN];
    int action;
    int sender_pid;
    double copy_duration;
};

int create_msg_queue();

bool is_msg_queue_empty(int msqid);

void delete_msg_queue(int msqid);

void send_msg(int msqid, int type, char *msg, int action, int sender_pid, double copy_duration, bool flag);

bool receive_msg(int msqid, struct msgbuf *temp, int type, bool flag);

int len_msg_queue(int msqid);

int get_last_sender(int msqid);

#endif