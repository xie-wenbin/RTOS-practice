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
#include "stm32f1xx.h"

#define KEYBOARD_MAX_KEYS (4)

#define    KEY1_GPIO_PORT    GPIOC            
#define    KEY1_GPIO_PIN     GPIO_PIN_0

#define    KEY2_GPIO_PORT    GPIOC         
#define    KEY2_GPIO_PIN     GPIO_PIN_1

#define    KEY3_GPIO_PORT    GPIOC         
#define    KEY3_GPIO_PIN     GPIO_PIN_2

typedef enum
{
    KEY1 = 0,
    KEY2,
    KEY3,
    KEY_NUMBER,
}KEY_ENUM;

enum
{
  KEYSTATE_PRESSED = 0,
  KEYSTATE_UNPRESS,
  KEYSTATE_NUM,
};



void KeyBoardInit(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
uint8_t Key_ScanNum(int iIndex);



#endif
