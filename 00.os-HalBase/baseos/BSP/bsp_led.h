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

#include "stm32f1xx_hal.h"

typedef enum
{
   LED1       = 0,  //
   LED2,


   LED_NUMBER,
}LED_ENUM;

void LED_BSP_Init(void);

void LED_On(int iLeds);
void LED_Off(int iLeds);
void LedToggle(uint8_t iIdx);



#endif /* __BSP_LED_H__ */
