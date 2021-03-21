/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);



/* Private defines -----------------------------------------------------------*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define APP_TASK_ROOT_PRIO    (tskIDLE_PRIORITY + 1)
#define APP_TASK_MAIN_PRIO    (APP_TASK_ROOT_PRIO + 1)

#define APP_TASK_SECOND_PRIO  (APP_TASK_MAIN_PRIO + 1)
#define APP_TASK_TASK2_PRIO   (APP_TASK_SECOND_PRIO + 1)


 
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/
#define  APP_TASK_ROOT_STK_SIZE        1024
#define  APP_TASK_MAIN_STK_SIZE        512

#define  APP_TASK_SECOND_STK_SIZE      512




#define BEEP_SUPPORT  1

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
