#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bsp.h"
#include "app_cfg.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "keyboard.h"

#include "vos.h"
#include "msg.h"
#include "vtask.h"
#include "timer.h"

/*
*************************************************************************
*                                        RTOS HANDLES
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static StackType_t AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];
static StackType_t AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];

static StaticTask_t AppTaskRootControl;
#endif
        
static vosThreadDef_t  vosRootThread;
UINT32 ulAppTicks ;

int gulSeconds;

void vApplicationTickHook( void )
{   
    ulAppTicks++;
    
    TimerProc();
}

void MainInit(void)
{
    BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();

    AddTimer(TIMER_CHECK_SECOND,OS_TMR_OPT_PERIODIC,configTICK_RATE_HZ,0xffff);

    printf("Ready to Go!\r\n");
}


UINT8 PidSelfProcess(Message *pMsg)
{
    switch(pMsg->msgHead.nMsgType)
    {
    case SELF_MSG_CODE_INIT:
        MainInit();
        break;
    case SELF_MSG_CODE_DELAY:
        break;
    }
    return 0;
}

void SecondTimer(void)
{
    gulSeconds++;

    LedToggle(0);

    printf("time:%d\r\n",gulSeconds);
}


UINT8 PidTimerProcess(Message *pMsg)
{
    UINT8 ucTimer = pMsg->msgHead.nMsgType;

    switch(ucTimer) /*MsgLen is userid */
    {
    case TIMER_CHECK_SECOND:
        SecondTimer();
        break;
    case TIMER_CHECK_ALARM:
        //Alarm(0);
        break;
    case TIMER_CHECK_BEEP:
        Beep(0);
        break;

    default:
        break;
    }
    
    return 0;
}

UINT8 Msg_proc(Message *pMsg)
{
    switch(pMsg->msgHead.nSendPid)
    {
    case PID_TIMER:
        return PidTimerProcess(pMsg);
    case PID_SELF:
        return PidSelfProcess(pMsg);
 

    default:
        break;
    }
      
    return 0;
}


/*****************************************************************
 * @brief:  Maintask send message , Secondtask recevie message
 * @param:  
 * @retval: 
 * @note:   
 ****************************************************************/
static void Main_Entry(Message *msg)
{	
    Msg_proc(msg);

}


void MainKickoff(void)
{
    Message *Msg;
    
    Msg = MessageAlloc(PID_SELF,0);
    if (Msg)
    {
        Msg->msgHead.nMsgType = SELF_MSG_CODE_INIT;
        MessageSend(Msg);
    }
}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

  VOS_CreateTask(VTASK_MAKE_PID(PID_MAIN_TASK,APP_TASK_MAIN_PRIO),(StackType_t *)&AppTaskMainStk[0],APP_TASK_MAIN_STK_SIZE,Main_Entry);
#else
  VOS_CreateTask(VTASK_MAKE_PID(PID_MAIN_TASK,APP_TASK_MAIN_PRIO),NULL,APP_TASK_MAIN_STK_SIZE,Main_Entry);
#endif
  MainKickoff();

  VOS_ThreadTerminate(vosRootThread.handle);

}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
 	
    BSP_Init(); 
    
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskRootControl,0,sizeof(AppTaskRootControl));
    vosRootThread.pTaskTcb = &AppTaskRootControl;
    vosRootThread.pBuffer  = AppTaskRootStk;
#endif

    vosRootThread.name = "Root";
    vosRootThread.pthread = AppTaskRoot;
    vosRootThread.tpriority = tskIDLE_PRIORITY+1; // in freeRTOS, task priority is from 0 up to max_prio,prio 0 is the idle task
    vosRootThread.stacksize = APP_TASK_ROOT_STK_SIZE;
    vosRootThread.handle    = VOS_ThreadCreate( &vosRootThread, NULL );

    if (vosRootThread.handle != NULL)
        vTaskStartScheduler();
    

    while(1);
}

