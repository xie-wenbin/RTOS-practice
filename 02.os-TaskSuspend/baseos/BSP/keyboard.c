#include "keyboard.h"



void KeyBoardInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
            
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure); 

}

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{           
    /*Check key is pressed*/
    if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEYSTATE_PRESSED )  
    {    
        /*wait key unpressed */
        while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEYSTATE_PRESSED);   
        return  KEYSTATE_PRESSED;    
    }
    else
        return KEYSTATE_UNPRESS;
}

