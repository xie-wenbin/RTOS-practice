#ifndef  __BSP_H__
#define  __BSP_H__

#include "vos.h"
#include "DTypeStm32.h"

#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


#define OS_TICKS_PER_SEC configTICK_RATE_HZ

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void BSP_Init(void);

void SysTick_DelayUs(int us);
void SysTick_DelayMs(int ms);
void delay_xms(u32 nms);

#define delay_ms SysTick_DelayMs
#define delay_us SysTick_DelayUs


#endif          

