#include <stdio.h>
#include <string.h>

#include "app_cfg.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "keyboard.h"

#include "vos.h"

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static StackType_t AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];
static StackType_t AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];
static StackType_t AppTaskSecondStk[APP_TASK_SECOND_STK_SIZE];
static StackType_t AppTaskTask2Stk[APP_TASK_TASK2_STK_SIZE];

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
static StaticTask_t AppTaskTask2;
#endif

static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;
static vosThreadDef_t  vosTask2;


static void Main_Task(void const *parameter)
{	
  while (1)
  {
    
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEYSTATE_PRESSED)
    {
        vTaskSuspend(vosSecondThread.handle);  //suspend second task
        printf("suspend second task run\r\n");
    }

    if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEYSTATE_PRESSED)
    {
        vTaskResume(vosSecondThread.handle);
        printf("resume second task run!\r\n");
    }
    
    vTaskDelay(100); 

  }
}

static void Second_Task(void const *parameter)
{	
    static float float_num=0.01;
    while (1)
    {
        LED_On(LED_2);
        float_num+=0.01f;
        taskENTER_CRITICAL();           
        printf("second task num value: %.4f\r\n",float_num);   
        taskEXIT_CRITICAL();          
        vTaskDelay(1000);
        LED_Off(LED_2);
        vTaskDelay(1000);
    }
}

static void Task_Task2(void const *p_arg)
{
    static float num = 0.01;
    while (1)
    {
        LED_On(LED_1);
        num+=0.01;
        taskENTER_CRITICAL();
        printf("task_task2 num value: %.4f\r\n",num);
        taskEXIT_CRITICAL();
        vTaskDelay(1000);
        LED_Off(LED_1);
        vTaskDelay(1000);
    }
}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;
    
    //BaseType_t xReturn = pdPASS;
    
    BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();

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
     
 #if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskTask2,0,sizeof(AppTaskTask2));
    vosTask2.pTaskTcb = &AppTaskTask2;
    vosTask2.pBuffer  = AppTaskTask2Stk;
#endif
     vosTask2.name = "Task_Task2";
     vosTask2.pthread = Task_Task2;
     vosTask2.tpriority = APP_TASK_TASK2_PRIO;
     vosTask2.stacksize = APP_TASK_TASK2_STK_SIZE;
     vosTask2.handle = VOS_ThreadCreate(&vosTask2,NULL);
     
     if (NULL != vosTask2.handle)
         printf("create Task_Task2 success!\r\n");

     VOS_ThreadTerminate(vosRootThread.handle);


}

int main(void)
{
		  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
    
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

