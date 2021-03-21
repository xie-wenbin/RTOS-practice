#include "bsp_led.h"

#define LED_GPIO_PORT  GPIOD
#define LED_GPIO_PIN   GPIO_PIN_12

#define LED2_GPIO_PORT  GPIOD
#define LED2_GPIO_PIN   GPIO_PIN_13

const uint16_t leds[LED_NUMBER] = {

    LED_GPIO_PIN,
    LED2_GPIO_PIN,
    
};

void LED_BSP_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOD_CLK_ENABLE(); 
    
    GPIO_InitStructure.Pin=LED_GPIO_PIN;
    GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull=GPIO_PULLUP;
    GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH; 
    HAL_GPIO_Init(LED_GPIO_PORT,&GPIO_InitStructure);
   
    GPIO_InitStructure.Pin=LED2_GPIO_PIN;
    HAL_GPIO_Init(LED2_GPIO_PORT,&GPIO_InitStructure);
    
	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN,GPIO_PIN_SET);
}

void LED_On(int iLeds)
{
	HAL_GPIO_WritePin(LED_GPIO_PORT, leds[iLeds],GPIO_PIN_RESET);
}

void LED_Off(int iLeds)
{
	HAL_GPIO_WritePin(LED_GPIO_PORT, leds[iLeds],GPIO_PIN_SET);
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


