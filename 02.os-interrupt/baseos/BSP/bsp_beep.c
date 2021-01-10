 /**
*****************************************************************
* @file: bsp_beep.c
* @author: wen  
* @date:  12/13/2020
* @brief:  beep ctrl code , beep pin in board F103VET6 is PD11
* @attention:
****************************************************************
 */
#include "bsp_beep.h"
#include "app_cfg.h"


#define BEEP_GPIO_PORT  GPIOD
#define BEEP_GPIO_PIN   GPIO_Pin_11
#define BEEP_GPIO_CLK   RCC_APB2Periph_GPIOD



void BeepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
}


void Beep(int On)
{
#if (BEEP_SUPPORT > 0)
    if (On)
    {
        GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
    else
    {
        GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
    }
#endif  
}


