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

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
#endif
        
static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;

#define USE_BINARYSEM

#ifdef USE_BINARYSEM

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t controlblock;
#endif

static vosSemDef_t vosBinary_Handle;
VOS_SEM  binary;

#else

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t semControlblock;
#endif
static vosSemDef_t vosSem_Handle;
VOS_SEM  sem;

#endif


#define LED1ON  1
#define LED1OFF 2
#define BEEPON  3
#define BEEPOFF 4
#define COMMANDERR  0XFF

void LowerToCap(u8 *str,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        if((96<str[i])&&(str[i]<123))   //
        str[i]=str[i]-32;               //trans to  capital letter 
    }
}

u8 CommandProcess(u8 *str)
{
    u8 CommandValue=COMMANDERR;
    if(strcmp((char*)str,"LED2ON")==0) CommandValue=LED1ON;
    else if(strcmp((char*)str,"LED2OFF")==0) CommandValue=LED1OFF;
    else if(strcmp((char*)str,"BEEPON")==0) CommandValue=BEEPON;
    else if(strcmp((char*)str,"BEEPOFF")==0) CommandValue=BEEPOFF;
    return CommandValue;
}

u8 buff[USART_REC_LEN];

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
    u8 ucLen = 0;
    u8 ucCmd = COMMANDERR;

    memset(&buff,0,sizeof(buff));
        
    while (1)
    {
      
#ifdef USE_BINARYSEM
        if (NULL != binary)
        {
            xReturn = VOS_SemGet(binary, portMAX_DELAY); //delay until sem pend
            
            if (pdTRUE == xReturn)
            {
                ucLen = USART_RX_STA&0x3fff;
                sprintf((char*)buff,"%s",USART_RX_BUF);
                buff[ucLen]='\0';
                LowerToCap(buff,ucLen);         
                ucCmd=CommandProcess(buff); 
                if(ucCmd!=COMMANDERR)
                {
                    printf("command is :%s\r\n",buff);
                    switch(ucCmd)                       
                    {
                        case LED1ON: 
                            LED_On(LED2);
                            break;
                        case LED1OFF:
                            LED_Off(LED2);
                            break;
                        case BEEPON:
                            Beep(1);
                            break;
                        case BEEPOFF:
                            Beep(0);
                            break;
                    }
                }
                else
                {
                    printf("invalid command!!\r\n");
                }

                USART_RX_STA=0;
                memset(USART_RX_BUF,0,USART_REC_LEN);
            }
            else
                printf("BinarySem_Handle wait failed:0x%lx\r\n",xReturn);
            
        }
#else
        if( Key_ScanNum(KEY3) == KEYSTATE_PRESSED )
        {
            if (NULL != sem)
            {
                xReturn = VOS_SemGet(sem, 0); //get immediately
                
                if (pdPASS == xReturn)
                    printf("key2 pressed, get 1 semphore count\r\n");
                else
                    printf("key2 pressed, can not get, 0x%x\r\n",xReturn);
                
                LED_On(LED1);
            }
        }
#endif
    }
}

static void Second_Task(void const *parameter)
{       
    while (1)
    {
        LedToggle(LED1);
        
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
    
    MainInit();

    taskENTER_CRITICAL();

#ifdef USE_BINARYSEM

#if(configSUPPORT_STATIC_ALLOCATION == 1)
    memset(&controlblock,0,sizeof(controlblock));
    vosBinary_Handle.pSemaphoreBuffer = &controlblock;
#endif
    vosBinary_Handle.dummy = 0;

    binary = VOS_CreateBinarySem(&vosBinary_Handle);

    if (NULL != binary)
    {
        printf("create BinarySem_Handle success!\r\n");
    }
#else
#if(configSUPPORT_STATIC_ALLOCATION == 1)
        memset(&semControlblock,0,sizeof(semControlblock));
        vosSem_Handle.pSemaphoreBuffer = &semControlblock;
#endif
        vosSem_Handle.dummy = 5; //max value 5

    sem = VOS_CreateSem(&vosSem_Handle, 5); // init 5 sem count
    
    if (NULL != sem)
    {
        printf("create CountSem_Handle success!\r\n");
    }
#endif

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

