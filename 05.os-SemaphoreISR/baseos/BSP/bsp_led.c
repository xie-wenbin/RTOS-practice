#include "bsp_led.h"

#define LED_GPIO_PORT  GPIOD
#define LED_GPIO_PIN   GPIO_Pin_12
#define LED_GPIO_CLK   RCC_APB2Periph_GPIOD

#define LED2_GPIO_PORT  GPIOD
#define LED2_GPIO_PIN   GPIO_Pin_13
#define LED2_GPIO_CLK   RCC_APB2Periph_GPIOD

const uint16_t leds[LED_NUMBER] = {

    LED_GPIO_PIN,
    LED2_GPIO_PIN,
    
};

void LED_BSP_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}

void LED_On(int iLeds)
{
	GPIO_ResetBits(LED_GPIO_PORT, leds[iLeds]);
}

void LED_Off(int iLeds)
{
	GPIO_SetBits(LED_GPIO_PORT, leds[iLeds]);
}

void LedToggle(uint8_t iIdx)
{
	if (iIdx == LED1)
	{
		LED_GPIO_PORT->ODR ^= LED_GPIO_PIN;
	}
	else if (iIdx == LED2)
	{
		LED2_GPIO_PORT->ODR ^= LED2_GPIO_PIN;
	}
}


void assert_failed(uint8_t* file, uint32_t line)
{
	LED_On(LED1);
}


