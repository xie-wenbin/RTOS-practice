 /**
*****************************************************************
* @file: keyboard.h
* @author: wen  
* @date:  12/13/2020
* @brief:  
* @attention:
****************************************************************
 */
#ifndef _KEY_BOARD_H_
#define _KEY_BOARD_H_
#include "stm32f10x.h"

#define KEYBOARD_MAX_KEYS (4)

#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY1_GPIO_PORT    GPIOC            
#define    KEY1_GPIO_PIN     GPIO_Pin_0

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY2_GPIO_PORT    GPIOC         
#define    KEY2_GPIO_PIN     GPIO_Pin_1

enum
{
  KEYSTATE_PRESSED = 0,
  KEYSTATE_UNPRESS,
  KEYSTATE_NUM,
};



void KeyBoardInit(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);



#endif
