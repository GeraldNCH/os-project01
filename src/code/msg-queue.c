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

// Creates a message queue.
// Returns the msqid.
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

// Check if message queue is empty.
// The return indicates if it's empty.
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

// Delete the specified message queue.
void delete_msg_queue(int msqid)
{
    if (msgctl(msqid, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
        exit(-1);
    }
}

// Sends a message in the specified queue.
void send_msg(int msqid, int type, int action, int sender_pid, char *msg, bool flag)
{
    struct msgbuf temp;
    temp.mtype = type;
    temp.action = action;
    temp.sender_pid = sender_pid;
    strcpy(temp.mtext, msg);

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
// The return value indicates if a message was received.
bool receive_msg(int msqid, struct msgbuf *temp, int type, int action, bool flag)
{
    if (flag)
    {
        return (msgrcv(msqid, temp, MAX_MSG_LEN, type, IPC_NOWAIT) == -1) ? false : true;
    }
    else
    {
        msgrcv(msqid, temp, MAX_MSG_LEN, type, 0);
        return true;
    }
}

// Get message queue len.
int len_msg_queue(int msqid)
{
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) != 0)
    {
        perror("msgctl");
        exit(-1);
    }
    return buf.msg_qnum;
}

// Get last message sender pid.
int get_last_sender(int msqid)
{
    struct msqid_ds buf;
    if (msgctl(msqid, IPC_STAT, &buf) != 0)
    {
        perror("msgctl");
        exit(-1);
    }
    return buf.msg_lspid;
}