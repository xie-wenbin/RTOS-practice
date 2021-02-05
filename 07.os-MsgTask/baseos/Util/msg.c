#include "stdint.h"
#include "string.h"

#include "memory.h"
#include "msg.h"
#include "voslog.h"
#include "vtask.h"

static MessageQueue gaMsgQueue[MAX_PID_NUMBER];
static int giMsgQueueMask;

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t semControlblock[MAX_PID_NUMBER];
#endif
static vosSemDef_t vosSem_Handle[MAX_PID_NUMBER];

void MessageQueueInit(void)
{
    int iLoop;

    giMsgQueueMask = 0;
    memset(gaMsgQueue,0,sizeof(gaMsgQueue));

    for(iLoop = 0; iLoop < MAX_PID_NUMBER;iLoop++)
    {
        gaMsgQueue[iLoop].miID = iLoop;
        gaMsgQueue[iLoop].sem = NULL;
    }
}

int MessageQueueIsFull(MessageQueue *queue)
{
    if (((queue->miFront + 1) % MAX_QUEUE_SIZE) == queue->miRear)
    {
        return 1;
    }

    return 0;
}

int MessageQueueEmpty(MessageQueue *queue)
{
    if (queue->miFront == queue->miRear)
    {
        return 1;
    }

    return 0;
}

MessageQueue* createMessageQueue(int iPid)
{
    int iLoop;
    MessageQueue *queue = &gaMsgQueue[iPid];

    // if queue already exist
    if (giMsgQueueMask & (1 << iPid))
    {
        return queue;
    }
    
#if(configSUPPORT_STATIC_ALLOCATION == 1)
    memset(&semControlblock[iPid],0,sizeof(semControlblock[iPid]));
    vosSem_Handle[iPid].pSemaphoreBuffer = &semControlblock[iPid];
#endif
    vosSem_Handle[iPid].dummy = MAX_QUEUE_SIZE;
    
    queue->sem = VOS_CreateSem(&vosSem_Handle[iPid], 0);

    if (NULL == queue->sem)
    {
        return NULL;
    }

    for (iLoop = 0; iLoop < MAX_QUEUE_SIZE; iLoop++)
    {
        queue->aMessages[iLoop] = NULL;
    }

    queue->miFront = 0;
    queue->miRear = 0;

    giMsgQueueMask |= (1 << iPid);

    return queue;
}

int queueMessage(MessageQueue *queue,Message *msg)
{
    uint32_t  cpu_sr = 0;

    if (MessageQueueIsFull(queue))
    {
        return -1;
    }

    OS_ENTER_CRITICAL();

    queue->aMessages[queue->miFront] = msg;

    queue->miFront = (queue->miFront + 1)% MAX_QUEUE_SIZE;

    OS_EXIT_CRITICAL();

    VOS_SemSend(queue->sem);

    return 0;
}

Message *waitMessage(MessageQueue *queue)
{
    Message *msg = NULL;
    
    uint32_t cpu_sr = 0;

    VOS_SemV(queue->sem);

    OS_ENTER_CRITICAL();

    if (MessageQueueEmpty(queue))
    {
        OS_EXIT_CRITICAL();
        return NULL;
    }

    msg = queue->aMessages[queue->miRear] ;

    queue->miRear = (queue->miRear + 1)% MAX_QUEUE_SIZE;

    OS_EXIT_CRITICAL();

    return msg;

}


void MessageQueueDestroy(MessageQueue *queue)
{

    if (NULL == queue)
    {
        return ;
    }

    if (!((1 << queue->miID) & (giMsgQueueMask)))
    {
        return ;
    }

    VOS_DestroySem(&queue->sem);
    giMsgQueueMask &= ~(1 << queue->miID);
    
}

Message *MessageAlloc(int pid,int MsgLen)
{
    Message *msg = (Message *)AllocMem(MsgLen + sizeof(Message)-1); 
    if (NULL == msg)
    {
        return 0;
    }

#ifdef VOS_DEBUG    
    // use for magic head
    msg->data[MsgLen] = 0xaa;
    msg->data[MsgLen+1] = 0xbb;
    msg->data[MsgLen+2] = 0xcc;
    msg->data[MsgLen+3] = 0xdd;
#endif
    msg->msgHead.nRcvPid = 0;
    msg->msgHead.nSendPid = (INT8U)pid;
    msg->msgHead.nMsgType = 0;
    msg->msgHead.MsgSeq   = 0;
    msg->msgHead.AddData  = 0;

    // fill length
    msg->msgHead.nMsgLen = MsgLen;
    
    return msg;
}

void MessageFree(Message *msg)
{
    // check msg magic

    if (NULL == msg)
    {
        return ;
    }

#ifdef VOS_DEBUG    
    if (msg->data[msg.msgHead.nMsgLen] != 0xaa
        || msg->data[msg.msgHead.nMsgLen+1] != 0xbb
        || msg->data[msg.msgHead.nMsgLen+2] != 0xcc
        || msg->data[msg.msgHead.nMsgLen+3] != 0xdd)
    {
        VOS_LOG(VOS_LOG_ERROR,"Message Head is corrupted");
    }
#endif    
    FreeMem(msg);
}


int MessageSend(Message *msg)
{
    int ret = -1;
    VOS_TASK * task =  VOS_GetTaskByPid(msg->msgHead.nRcvPid );

    if (NULL != task)
    {
        ret = queueMessage(task->queue,msg);
    }

    if (-1 == ret)
    {
        MessageFree(msg);
    }

    return ret;
}


