#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define APP_TASK_ROOT_PRIO    (tskIDLE_PRIORITY + 1)


 
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define  APP_TASK_ROOT_STK_SIZE        512
#define  APP_TASK_MAIN_STK_SIZE        512

#define  APP_TASK_SECOND_STK_SIZE      512
#define  APP_TASK_HIGH_STK_SIZE        512



#define	TIMER_CHECK_SECOND                     0 

#define	TIMER_CHECK_KEY                        1 

#define	TIMER_CHECK_BEEP                       2 

#define	TIMER_CHECK_ALARM                      3 

typedef enum
{
   SELF_MSG_CODE_INIT = 0,
   SELF_MSG_CODE_DELAY,

   // user add here
   SELF_MSG_CODE_USER_BEGIN = 0X10,
   SELF_MSG_CODE_USER_CANCMD,
   SELF_MSG_CODE_NUM,
}SELF_MSG_CODE_ENUM;



#define BEEP_SUPPORT  1


#endif // __APP_CFG_H__

