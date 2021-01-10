#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define APP_TASK_ROOT_PRIO    (tskIDLE_PRIORITY + 1)
#define APP_TASK_MAIN_PRIO    (APP_TASK_ROOT_PRIO + 1)

#define APP_TASK_SECOND_PRIO  (APP_TASK_MAIN_PRIO + 1)
#define APP_TASK_TASK2_PRIO   (APP_TASK_SECOND_PRIO + 1)
#define APP_TASK_HIGH_PRIO    (APP_TASK_TASK2_PRIO + 1)


 
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define  APP_TASK_ROOT_STK_SIZE        2048
#define  APP_TASK_MAIN_STK_SIZE        512

#define  APP_TASK_SECOND_STK_SIZE      512
#define  APP_TASK_HIGH_STK_SIZE        512




#define BEEP_SUPPORT  1


#endif // __APP_CFG_H__

