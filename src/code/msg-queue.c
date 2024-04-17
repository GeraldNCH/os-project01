#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../headers/msg-queue.h"

int create_msg_queue()
{
    key_t msqkey = 999;
    int msqid;

    // Delete queue if already exists
    if ((msqid = msgget(msqkey, 0666)) != -1)
    {
        if (msgctl(msqid, IPC_RMID, NULL) != 0)
        {
            perror("msgctl");
            exit(-1);
        }
    }

    // Create msg queue and return ID
    msqid = msgget(msqkey, IPC_CREAT | S_IRUSR | S_IWUSR);
    return msqid;
}

bool is_msg_queue_empty(int msqid)
{
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) != 0)
    {
        perror("msgctl");
        exit(-1);
    }
    return (buf.msg_qnum == 0) ? true : false;
}

void delete_msg_queue(int msqid)
{
    if (msgctl(msqid, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
        exit(-1);
    }
}

void send_msg(int msqid, long type, char *msg, bool flag)
{
    struct msgbuf temp;
    temp.mtype = type;
    strcpy(temp.mtext, msg);

    printf("Sent message: %s\n", temp.mtext);

    if (flag)
    {
        if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), IPC_NOWAIT) != 0)
        {
            perror("msgsnd");
        }
    }
    else
    {
        if (msgsnd(msqid, (void *)&temp, sizeof(temp.mtext), 0) != 0)
        {
            perror("msgsnd");
        }
    }
}

// Receive a message from a message queue.
// The returned string has to be freed.
void receive_msg(int msqid, struct msgbuf *temp, long type, bool flag)
{
    if (flag)
    {
        (msgrcv(msqid, temp, MAX_MSG_LEN, type, IPC_NOWAIT) != 0);
    }
    else
    {
        (msgrcv(msqid, temp, MAX_MSG_LEN, type, 0) != 0);
    }
    printf("Received message: %s\n", (*temp).mtext);
}