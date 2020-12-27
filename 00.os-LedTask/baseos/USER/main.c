#include <stdio.h>
#include <string.h>

#include "app_cfg.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "keyboard.h"

#include "vos.h"


static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;

static void Main_Task(void const *parameter)
{	
  while (1)
  {
    LED_On(LED_1);
    vTaskDelay(200);  
    
    LED_Off(LED_1);
    vTaskDelay(800); 
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
        printf("float_num value: %.4f\r\n",float_num);   
        taskEXIT_CRITICAL();          
        vTaskDelay(1000);
        LED_Off(LED_2);
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
    

#if 0
     taskENTER_CRITICAL();           //进入临界区
     /* 创建LED_Task任务 */
     xReturn = xTaskCreate((TaskFunction_t )Main_Task, /* 任务入口函数 */
                           (const char*    )"LED_Task",/* 任务名字 */
                           (uint16_t       )APP_TASK_MAIN_STK_SIZE,   /* 任务栈大小 */
                           (void*          )NULL,  /* 任务入口函数参数 */
                           (UBaseType_t    )APP_TASK_MAIN_PRIO,     /* 任务的优先级 */
                           (TaskHandle_t*  )&vosMainThread.handle);/* 任务控制块指针 */
     if(pdPASS == xReturn)
       printf("创建LED_Task任务成功!\r\n");
     /* 创建KEY_Task任务 */
     xReturn = xTaskCreate((TaskFunction_t )Second_Task,  /* 任务入口函数 */
                           (const char*    )"KEY_Task",/* 任务名字 */
                           (uint16_t       )APP_TASK_SECOND_STK_SIZE,  /* 任务栈大小 */
                           (void*          )NULL,/* 任务入口函数参数 */
                           (UBaseType_t    )APP_TASK_SECOND_PRIO, /* 任务的优先级 */
                           (TaskHandle_t*  )&vosSecondThread.handle);/* 任务控制块指针 */ 
     if(pdPASS == xReturn)
       printf("创建KEY_Task任务成功!\r\n");
     
     vTaskDelete(vosRootThread.handle); //删除AppTaskCreate任务
     
     taskEXIT_CRITICAL();            //退出临界区
#else

    /* create Main_Task */
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

     VOS_ThreadTerminate(vosRootThread.handle);
#endif


}

int main(void)
{
		  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

    vosRootThread.name = "Root";
    vosRootThread.pthread = AppTaskRoot;
    vosRootThread.tpriority = tskIDLE_PRIORITY+1; // in freeRTOS, task priority is from 0 up to max_prio,prio 0 is the idle task
    vosRootThread.stacksize = APP_TASK_ROOT_STK_SIZE;
    vosRootThread.handle    = VOS_ThreadCreate( &vosRootThread, NULL );

    if (vosRootThread.handle != NULL)
        vTaskStartScheduler();
    

    while(1);
}

