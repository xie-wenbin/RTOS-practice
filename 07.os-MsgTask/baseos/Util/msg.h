#ifndef _MSG_H_
#define _MSG_H_
 /**
*****************************************************************
* @file: msg.h
* @author: wen  
* @date:  1/10/2021
* @brief:  
* @attention:
****************************************************************
 */

#include "vos.h"

#define MAX_QUEUE_SIZE  (32)

typedef struct
{
    INT8U nSendPid;
    INT8U nRcvPid;
    INT8U nMsgType;
    INT8U MsgSeq;
    INT16U AddData;
    INT16U nMsgLen;
}MsgHead;

typedef struct
{
    MsgHead msgHead;
    
    char data[1];  //pointer of data msg
}Message;

typedef struct
{
    Message *aMessages[MAX_QUEUE_SIZE];
    VOS_SEM sem;
    int miFront;
    int miRear;
    int miID;
    
}MessageQueue;


void MessageQueueInit(void);
int MessageQueueIsFull(MessageQueue *queue);
int MessageQueueEmpty(MessageQueue *queue);
Message *waitMessage(MessageQueue *queue);

MessageQueue *createMessageQueue(int pid);
int queueMessage(MessageQueue *queue,Message *msg);
void dumpMessage(MessageQueue *queue);

void MessageFree(Message *msg);
int MessageSend(Message *msg);
void MessageQueueDestroy(MessageQueue *queue);
Message *MessageAlloc(int pid,int MsgLen);

#endif// _MSG_H_

