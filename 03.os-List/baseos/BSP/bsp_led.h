 /**
*****************************************************************
* @file: bsp_led.h
* @author: wen  
* @date:  12/3/2020
* @brief:  work for led show
* @attention: none
****************************************************************
 */
#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

typedef enum
{
   LED_1       = 0,  //
   LED_2,


   LED_NUMBER,
}LED_ENUM;

void LED_BSP_Init(void);

void LED_On(int iLeds);
void LED_Off(int iLeds);



#endif /* __BSP_LED_H__ */
