#ifndef __GPIO_H__
#define __GPIO_H__

/**
  ******************************************************************************
  * @file    gpio.h 
  * @author  STM32 Team
  * @version V1.0.0
  * @date    10/04/2016
  * @brief   Interface for GPIO & Ext interrupt access.
  *          
  ******************************************************************************
  *
  *
  */ 

#define STM32F103_GPIO_BASE_SPACE 0

/* GPIO ports num */
#define STM32F103_GPIO_GPA_N (16)
#define STM32F103_GPIO_GPB_N (16)
#define STM32F103_GPIO_GPC_N (16)
#define STM32F103_GPIO_GPD_N (16)
#define STM32F103_GPIO_GPE_N (16)
#define STM32F103_GPIO_GPF_N (16)
#define STM32F103_GPIO_GPG_N (16)

#define STM32F103_GPIO_MASK (0xFUL)

#define STM32F103_GPIO_NEXT(__gpio) ((__gpio##_START) + (__gpio##_N) + STM32F103_GPIO_BASE_SPACE)


enum stm32f103_gpio_number {
    STM32F103_GPIO_GPA_START = 0,
    STM32F103_GPIO_GPB_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPA),
    STM32F103_GPIO_GPC_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPB),
    STM32F103_GPIO_GPD_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPC),
	STM32F103_GPIO_GPE_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPD),
	STM32F103_GPIO_GPF_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPE),
	STM32F103_GPIO_GPG_START = STM32F103_GPIO_NEXT(STM32F103_GPIO_GPF),
}

 /* STM32F103 GPIO number definitions. */
 
#define STM32F103_GPA(_nr)	(STM32F103_GPIO_GPA_START + (_nr))
#define STM32F103_GPB(_nr)	(STM32F103_GPIO_GPB_START + (_nr))
#define STM32F103_GPC(_nr)	(STM32F103_GPIO_GPC_START + (_nr))
#define STM32F103_GPD(_nr)	(STM32F103_GPIO_GPD_START + (_nr))
#define STM32F103_GPE(_nr)	(STM32F103_GPIO_GPE_START + (_nr))
#define STM32F103_GPF(_nr)	(STM32F103_GPIO_GPF_START + (_nr))
#define STM32F103_GPG(_nr)	(STM32F103_GPIO_GPG_START + (_nr))

#endif // __GPIO_H__


