#include "keyboard.h"

static GPIO_TypeDef* KeyPort[KEY_NUMBER] = {
    KEY1_GPIO_PORT,
    KEY2_GPIO_PORT,
    KEY3_GPIO_PORT,
};

static uint16_t KeyPin[KEY_NUMBER] = {
    KEY1_GPIO_PIN,
    KEY2_GPIO_PIN,
    KEY3_GPIO_PIN,
};

void KeyBoardInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE(); 
            
    GPIO_Initure.Pin   = KEY1_GPIO_PIN | KEY2_GPIO_PIN | KEY3_GPIO_PIN;            
    GPIO_Initure.Mode  =GPIO_MODE_INPUT;      
    GPIO_Initure.Pull  =GPIO_PULLUP;        
    GPIO_Initure.Speed =GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{           
    /*Check key is pressed*/
    if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEYSTATE_PRESSED )  
    {    
        /*wait key unpressed */
        while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEYSTATE_PRESSED);   
        return  KEYSTATE_PRESSED;    
    }
    else
        return KEYSTATE_UNPRESS;
}

uint8_t Key_ScanNum(int iIndex)
{
    if (iIndex >= KEY_NUMBER)
    {
        return KEYSTATE_UNPRESS;
    }
    
    /*Check key is pressed*/
    if(HAL_GPIO_ReadPin(KeyPort[iIndex],KeyPin[iIndex]) == KEYSTATE_PRESSED )  
    {    
        /*wait key unpressed */
        while(HAL_GPIO_ReadPin(KeyPort[iIndex],KeyPin[iIndex]) == KEYSTATE_PRESSED);   
        return  KEYSTATE_PRESSED;    
    }
    else
        return KEYSTATE_UNPRESS;
}

