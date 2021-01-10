#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bsp.h"
#include "app_cfg.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "keyboard.h"
#include "uartTimer.h"

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

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
#endif
    
static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;

static vosQueueDef_t vosMsgQueue;
static vosQueueDef_t vosUartQueue;
VOS_QUEUE    Msg_Queue;
VOS_QUEUE    Uart_Queue;

/*
*************************************************************************
*                                        DEFINES
*************************************************************************
*/
#define  QUEUE_LEN    4   
#define  QUEUE_SIZE   4   

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static uint8_t pucQueueBuff[QUEUE_LEN*QUEUE_SIZE];
static StaticQueue_t pQueueCtrl;

static uint8_t pucUartBuff[USART_REC_LEN];
static StaticQueue_t pUartQueueCtrl;

#endif

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
    
    uint32_t recv; 
    
    while (1)
    {
      if (NULL != Msg_Queue)
      {
          void* pAdr = &recv;
          
          xReturn = VOS_QueueV(Msg_Queue,portMAX_DELAY,&pAdr); //delay until msg recevied
          
          if (pdPASS == xReturn)
              printf("Main_Task:message recevie:%d\r\n",recv);
          else
              printf("Main_Task:msg wait failed:0x%lx\r\n",xReturn);
      }
    }
}

static void Second_Task(void const *parameter)
{	
    BaseType_t xReturn = pdPASS;
    uint32_t send_data1 = 1;
    uint32_t send_data2 = 2;
    
    while (1)
    {
        if (NULL != Msg_Queue)
        {
            if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEYSTATE_PRESSED)
            {
                printf("send message,send_data1！\r\n");
                xReturn = VOS_QueueP( Msg_Queue, 0, &send_data1);        
                if(pdPASS == xReturn)
                printf("message send_data1 success!\r\n\r\n");
            }
            
            if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEYSTATE_PRESSED )
            {
              printf("send message,send_data2！\r\n");
              xReturn = VOS_QueueP( Msg_Queue, 0, &send_data2);       
              if(pdPASS == xReturn)
                printf("message send_data2 success!\r\n\r\n");
            }
            
        }

        vTaskDelay(20);
    }
}

void MainInit(void)
{
    BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();
    TIM2_Int_Init(5000,7200-1);

}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;
    
    //BaseType_t xReturn = pdPASS;

    MainInit();

    taskENTER_CRITICAL();
    
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&pQueueCtrl,0,sizeof(pQueueCtrl));
    vosMsgQueue.pStaticQueue = &pQueueCtrl;
    vosMsgQueue.pucQueueStorage = pucQueueBuff;
#endif
    vosMsgQueue.length = QUEUE_LEN;

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&pUartQueueCtrl,0,sizeof(pUartQueueCtrl));
    vosUartQueue.pStaticQueue = &pUartQueueCtrl;
    vosUartQueue.pucQueueStorage = pucUartBuff;
#endif
    vosUartQueue.length = QUEUE_LEN;


    Msg_Queue = VOS_CreateQueue(&vosMsgQueue,sizeof(uint32_t));
    Uart_Queue = VOS_CreateQueue(&vosUartQueue,USART_REC_LEN * sizeof(uint8_t));
        
    if (NULL != Msg_Queue)
    {
        printf("创建Msg_Queue成功!\r\n");
    }

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

     VOS_ThreadTerminate(vosRootThread.handle);
     
     taskEXIT_CRITICAL();

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

