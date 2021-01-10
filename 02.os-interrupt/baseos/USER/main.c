#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "app_cfg.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "keyboard.h"

#include "Timer.h"

#include "vos.h"

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static StackType_t AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];
static StackType_t AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];
static StackType_t AppTaskSecondStk[APP_TASK_SECOND_STK_SIZE];

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
#endif

static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
//static vosThreadDef_t  vosSecondThread;


static void Main_Task(void const *parameter)
{	
    static u32 ulCount=0;
    while (1)
    {
        ulCount+=1;
        
        LED_On(LED_2);     
        vTaskDelay(500);
        LED_Off(LED_2);
        vTaskDelay(500);
                
        if (5 == ulCount)
        {
            printf("close interrupt..........\r\n");
            portDISABLE_INTERRUPTS();
            
            delay_xms(5000);

            printf("continue task,open interrupt............\r\n");
            portENABLE_INTERRUPTS();
            
        }
        
    }
}

void MainInit(void)
{
    BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();
    TIM3_Int_Init(10000-1,7200-1);  // 1 second period
    TIM5_Int_Init(10000-1,7200-1);

}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;
    
    //BaseType_t xReturn = pdPASS;
   
    MainInit();

    taskENTER_CRITICAL();

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
        
    taskEXIT_CRITICAL();

#if 0
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
#endif

     VOS_ThreadTerminate(vosRootThread.handle);


}

int main(void)
{
		  
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
    
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

