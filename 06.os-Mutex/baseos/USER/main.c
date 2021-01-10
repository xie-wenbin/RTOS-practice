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


/*
*************************************************************************
*                                        RTOS HANDLES
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static StackType_t AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];
static StackType_t AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];
static StackType_t AppTaskSecondStk[APP_TASK_SECOND_STK_SIZE];
static StackType_t AppTaskHighStk[APP_TASK_HIGH_STK_SIZE];

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
static StaticTask_t AppTaskHighControl;
#endif
        
static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;
static vosThreadDef_t  vosHighThread;

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t mutexControlblock;
#endif
static vosMutexDef_t vosMutex_Handle;

VOS_MUTEX mutex =NULL;


/*****************************************************************
 * @brief:  Maintask send message , Secondtask recevie message
 * @param:  
 * @retval: 
 * @note:   
 ****************************************************************/
static void Main_Task(void const *parameter)
{	
    (void)parameter;

    BaseType_t xReturn = pdPASS;
    int i;
        
    while (1)
    {
        printf("LowPriority_Task get mutex semaphore\r\n");

        xReturn = VOS_MutexGet(mutex,portMAX_DELAY);
        
        if(pdTRUE == xReturn)
        {
            printf("LowPriority_Task Runing\r\n\r\n");
        
            LED_Off(LED2);
        }
        else
            printf("LowPriority_Task get mutex fail\r\n");
        
        
        for(i=0;i<2000000;i++)
        {
            taskYIELD();//run task sheduler
        }


        printf("LowPriority_Task ÊÍ·Å»¥³âÁ¿!\r\n");
        xReturn = VOS_MutexSend( mutex );

        vTaskDelay(1000);

    }
}

static void Second_Task(void const *parameter)
{	    
    while (1)
    {
        printf("MidPriority_Task Runing\r\n");
        vTaskDelay(1000);
        
        if (Key_ScanNum(KEY1) == KEYSTATE_PRESSED)
        {
            printf("send message£¡\r\n\r\n");

        }
    }

}

static void HighPrioTask(void const *parameter)
{
    BaseType_t xReturn = pdPASS;

    while (1)
    {
        printf("HighPriority_Task get mutex semaphore\r\n");

		xReturn = VOS_MutexGet(mutex,portMAX_DELAY);
        if(pdTRUE == xReturn)
        {
            printf("HighPriority_Task Runing\r\n");
            
            LED_On(LED2);
        }
        else
          printf("HighPriority_Task get mutex fail\r\n");
        
        
        printf("HighPriority_Task give mutex semaphore!\r\n\r\n");
        xReturn = VOS_MutexSend( mutex);
    
      
        vTaskDelay(1000);
    }
}


void MainInit(void)
{
    BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();

}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;
    
    BaseType_t xReturn = pdPASS;
    
    MainInit();

    taskENTER_CRITICAL();
    
#if(configSUPPORT_STATIC_ALLOCATION == 1)
    memset(&mutexControlblock,0,sizeof(mutexControlblock));
    vosMutex_Handle.controlblock= &mutexControlblock;
#endif
    vosMutex_Handle.dummy = 0; 

    mutex = VOS_MutexCreate(&vosMutex_Handle);
    if (NULL != mutex)
    {
        printf("create MuxSem_Handle success!\r\n");
    }

     xReturn = VOS_MutexSend(mutex);
     
     if (pdTRUE == xReturn)
        printf("give mutex semaphore success\r\n");

    /* create Main_Task */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskMainControl,0,sizeof(AppTaskMainControl));
    vosMainThread.pTaskTcb = &AppTaskMainControl;
    vosMainThread.pBuffer  = AppTaskMainStk;
#endif

    vosMainThread.name = "Main_Task";
    vosMainThread.pthread = Main_Task;
    vosMainThread.tpriority = APP_TASK_MAIN_PRIO;
    vosMainThread.stacksize = APP_TASK_MAIN_STK_SIZE;
    vosMainThread.handle = VOS_ThreadCreate(&vosMainThread,NULL);

    if (NULL != vosMainThread.handle)
        printf("create Main_Task success!\r\n");
        
     /*create Second_Task */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskSecondControl,0,sizeof(AppTaskSecondControl));
    vosSecondThread.pTaskTcb = &AppTaskSecondControl;
    vosSecondThread.pBuffer  = AppTaskSecondStk;
#endif
     vosSecondThread.name = "Second_Task";
     vosSecondThread.pthread = Second_Task;
     vosSecondThread.tpriority = APP_TASK_SECOND_PRIO;
     vosSecondThread.stacksize = APP_TASK_SECOND_STK_SIZE;
     vosSecondThread.handle = VOS_ThreadCreate(&vosSecondThread,NULL);
     
     if (NULL != vosSecondThread.handle)
         printf("create Second_Task success!\r\n");

    /*create high priority task*/
 #if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskHighControl,0,sizeof(AppTaskHighControl));
    vosHighThread.pTaskTcb = &AppTaskHighControl;
    vosHighThread.pBuffer  = AppTaskHighStk;
#endif
     vosHighThread.name = "HighPrio_Task";
     vosHighThread.pthread = HighPrioTask;
     vosHighThread.tpriority = APP_TASK_HIGH_PRIO;
     vosHighThread.stacksize = APP_TASK_HIGH_STK_SIZE;
     vosHighThread.handle = VOS_ThreadCreate(&vosHighThread,NULL);
     
     if (NULL != vosSecondThread.handle)
         printf("create HighPrio_Task success!\r\n");
     
     taskEXIT_CRITICAL();

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

