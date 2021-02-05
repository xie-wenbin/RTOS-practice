#include <string.h>
#include <stdio.h>
#include "vtask.h"
#include "msg.h"
#include "memory.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticTask_t gaTaskControlblock[MAX_PID_NUMBER];
#endif

#define VOS_TASK_BASE_PRIROITY (APP_TASK_MAIN_PRIO)

static VOS_TASK gaTasks[MAX_PID_NUMBER];

static int giTaskMask;

void VOS_TaskInit(void)
{
    
    giTaskMask = 0;

    memset(gaTasks,0,sizeof(gaTasks));

}

void VOS_TaskEntry(void *arg)
{
    VOS_TASK *task = (VOS_TASK *)arg;

    for (;;) 
    {

        Message *msg = waitMessage(task->queue);

        if (msg) // should be valid
        {
            (*task->entry)(msg);
            MessageFree(msg);
        }
    }
}

VOS_TASK *VOS_CreateTask(int Pid,StackType_t *ptos,uint32_t stk_size,TaskEntry entry)
{
    VOS_TASK *task = NULL;
    char *taskname;
    
    int iPid = (Pid & 0xFF);

    taskname = (char *)AllocMem(16*sizeof(char));

    //iPid -= VOS_TASK_BASE_PRIROITY;


    if (iPid >= MAX_PID_NUMBER
        || iPid < 0)
    {
        return NULL;
    }

    if (giTaskMask & (1 << iPid))
    {
        return &gaTasks[iPid];
    }

    task = &gaTasks[iPid];

    memset(task,0,sizeof (VOS_TASK));

    task->queue = createMessageQueue(iPid);

    if (NULL == task->queue)
    {
        return NULL;
    }
    task->iPid = iPid;

    task->entry = entry;

    sprintf(taskname,"Task%d",iPid);
    
    {
            
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

    memset(&gaTaskControlblock[iPid],0,sizeof(gaTaskControlblock[iPid]));

    task->handle = xTaskCreateStatic((TaskFunction_t )VOS_TaskEntry,
                                (const char*    )taskname,
                                stk_size,
                                task,
                                ((Pid >> 16) & 0xff),
                                ptos,
                                &gaTaskControlblock[iPid]);

    
#else
              xTaskCreate((TaskFunction_t )VOS_TaskEntry,
                          (const char*    )taskname,
                          (uint16_t )stk_size,
                          task,
                          ((Pid >> 16) & 0xff),
                          &task->handle);
    
#endif

        if (task->handle == NULL)
        {

            MessageQueueDestroy(task->queue);

            return NULL;
        }
        task->iPid = iPid;

        giTaskMask |= (1 << iPid);
    }
	return task;
}

void VOS_DestroyTask(VOS_TASK *task)
{
    if (!(giTaskMask & (1 << task->iPid)))
    {
        return;
    }

    vTaskDelete(task->handle);

    //OSTaskDel(task->Pid + VOS_TASK_BASE_PRIROITY);

    
    giTaskMask &= ~(1 << task->iPid);
}



VOS_TASK *VOS_GetTaskByPid(int Pid)
{
    if (Pid <0 || Pid >= MAX_PID_NUMBER)
    {
        return NULL;
    }
    return &gaTasks[Pid];
}

MessageQueue *VOS_GetTaskQueueByPid(int Pid)
{
    if (Pid <0 || Pid >= MAX_PID_NUMBER)
    {
        return NULL;
    }
    
    if (NULL == &gaTasks[Pid])
    {
        return NULL;
    }

    return gaTasks[Pid].queue;
}

