#ifndef _VTASK_H_
#define _VTASK_H_
 /**
*****************************************************************
* @file: vtask.h
* @author: wen  
* @date:  1/12/2021
* @brief:  
* @attention:
****************************************************************
 */


#include "msg.h"


#define MAX_PID_NUMBER    3

typedef void (*TaskEntry)(Message *Msg);

typedef struct
{
    int iPid;
    MessageQueue *queue;
    TaskEntry entry;
    TaskHandle_t handle;
}VOS_TASK;


typedef enum 
{
   PID_MAIN_TASK = 0, // user defined proc
   PID_NETITF_TASK,   // user defined proc
   PID_DUMMY2_TASK,   // user defined proc
   PID_NUM,
}TASK_PID;


#define  APP_TASK_MAIN_PRIO     4  // DO NOT CHANGE!! 

#define VTASK_MAKE_PID(pid,pri)((pri<<16)|pid)

#define    PID_TIMER  (MAX_PID_NUMBER+0)
#define    PID_RS485  (MAX_PID_NUMBER+1)
#define    PID_SERIAL (MAX_PID_NUMBER+2)
#define    PID_USB    (MAX_PID_NUMBER+3)
#define    PID_KB     (MAX_PID_NUMBER+4)
#define    PID_CAN    (MAX_PID_NUMBER+5)
#define    PID_ADC    (MAX_PID_NUMBER+6)
#define    PID_SELF   (MAX_PID_NUMBER+7)
#define    PID_SAPP   (MAX_PID_NUMBER+8)
#define    PID_RFID   (MAX_PID_NUMBER+9)
#define    PID_TSC2007 (MAX_PID_NUMBER+10)

typedef enum
{
   SYSTEM_MSG_CODE_INIT = 0,
   SYSTEM_MSG_CODE_DELAY,
   SYSTEM_MSG_CODE_TSS,
}SYSTEM_MSG_CODE;



VOS_TASK *VOS_GetTaskByPid(int Pid);

void VOS_TaskInit(void);

VOS_TASK *VOS_CreateTask(int Pid,StackType_t  *ptos,uint32_t   stk_size,TaskEntry entry);

void VOS_DumpTask(int Pid);




#endif// _VTASK_H_

