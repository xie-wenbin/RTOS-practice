#include <stdio.h>
#include <string.h>

#include "bsp.h"
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

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
#endif

static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
//static vosThreadDef_t  vosSecondThread;

List_t TestList;		//测试用列表
ListItem_t ListItem1;	//测试用列表项1
ListItem_t ListItem2;	//测试用列表项2
ListItem_t ListItem3;	//测试用列表项3

static void Main_Task(void const *parameter)
{	
    vListInitialise(&TestList);
    vListInitialiseItem(&ListItem1);
    vListInitialiseItem(&ListItem2);
    vListInitialiseItem(&ListItem3);

    ListItem1.xItemValue=40;            //ListItem1列表项值为40
    ListItem2.xItemValue=60;            //ListItem2列表项值为60
    ListItem3.xItemValue=50;            //ListItem3列表项值为50


    printf("/*******************列表和列表项地址*******************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList                          %#x                   \r\n",(int)&TestList);
    printf("TestList->pxIndex                 %#x                   \r\n",(int)TestList.pxIndex);
    printf("TestList->xListEnd                %#x                   \r\n",(int)(&TestList.xListEnd));
    printf("ListItem1                         %#x                   \r\n",(int)&ListItem1);
    printf("ListItem2                         %#x                   \r\n",(int)&ListItem2);
    printf("ListItem3                         %#x                   \r\n",(int)&ListItem3);
    printf("/************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    
    while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)!=KEYSTATE_PRESSED) delay_ms(10);   

    vListInsert(&TestList,&ListItem1);      //插入列表项ListItem1
    printf("/******************添加列表项ListItem1*****************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x                   \r\n",(int)(ListItem1.pxNext));
    printf("/*******************前后向连接分割线********************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x                   \r\n",(int)(ListItem1.pxPrevious));
    printf("/************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    
    while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)!=KEYSTATE_PRESSED) delay_ms(10);   

    vListInsert(&TestList,&ListItem2);  //插入列表项ListItem2
    printf("/******************添加列表项ListItem2*****************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x                   \r\n",(int)(ListItem1.pxNext));
    printf("ListItem2->pxNext                 %#x                   \r\n",(int)(ListItem2.pxNext));
    printf("/*******************前后向连接分割线********************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x                   \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious             %#x                   \r\n",(int)(ListItem2.pxPrevious));
    printf("/************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    
    while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)!=KEYSTATE_PRESSED) delay_ms(10);   
    
    //第五步：向列表TestList添加列表项ListItem3，并通过串口打印所有
    //列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
    //项在列表中的连接情况。
    vListInsert(&TestList,&ListItem3);  //插入列表项ListItem3
    printf("/******************添加列表项ListItem3*****************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x                   \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x                   \r\n",(int)(ListItem3.pxNext));
    printf("ListItem2->pxNext                 %#x                   \r\n",(int)(ListItem2.pxNext));
    printf("/*******************前后向连接分割线********************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x                   \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x                   \r\n",(int)(ListItem3.pxPrevious));
    printf("ListItem2->pxPrevious             %#x                   \r\n",(int)(ListItem2.pxPrevious));
    printf("/************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    
    while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)!=KEYSTATE_PRESSED) delay_ms(10);   
    
    //第六步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
    //pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
    uxListRemove(&ListItem2);                       //删除ListItem2
    printf("/******************删除列表项ListItem2*****************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x                   \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x                   \r\n",(int)(ListItem3.pxNext));
    printf("/*******************前后向连接分割线********************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x                   \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x                   \r\n",(int)(ListItem3.pxPrevious));
    printf("/************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    
    while(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)!=KEYSTATE_PRESSED) delay_ms(10);   
    
    //第七步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
    //pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
    TestList.pxIndex=TestList.pxIndex->pxNext;          //pxIndex向后移一项，这样pxIndex就会指向ListItem1。
    vListInsertEnd(&TestList,&ListItem2);               //列表末尾添加列表项ListItem2
    printf("/***************在末尾添加列表项ListItem2***************/\r\n");
    printf("项目                              地址                  \r\n");
    printf("TestList->pxIndex                 %#x                   \r\n",(int)TestList.pxIndex);
    printf("TestList->xListEnd->pxNext        %#x                   \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem2->pxNext                 %#x                   \r\n",(int)(ListItem2.pxNext));
    printf("ListItem1->pxNext                 %#x                   \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x                   \r\n",(int)(ListItem3.pxNext));
    printf("/*******************前后向连接分割线********************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x                   \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem2->pxPrevious             %#x                   \r\n",(int)(ListItem2.pxPrevious));
    printf("ListItem1->pxPrevious             %#x                   \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x                   \r\n",(int)(ListItem3.pxPrevious));
    printf("/************************结束**************************/\r\n\r\n\r\n");

    while (1)
    {        
        LED_On(LED_2);     
        delay_xms(1000);
        LED_Off(LED_2);
        delay_xms(1000);        
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

