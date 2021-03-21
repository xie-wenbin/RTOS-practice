/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "bsp.h"
#include "bsp_led.h"
#include "UartCmd.h"
#include "keyboard.h"

#include "vos.h"

//#include "gpio.h"
/*
*************************************************************************
*                                        RTOS HANDLES
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
static StackType_t AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];
static StackType_t AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];
static StackType_t AppTaskSecondStk[APP_TASK_SECOND_STK_SIZE];

static StaticTask_t AppTaskRootControl;
static StaticTask_t AppTaskMainControl;
static StaticTask_t AppTaskSecondControl;
#endif
        
static vosThreadDef_t  vosRootThread;
static vosThreadDef_t  vosMainThread;
static vosThreadDef_t  vosSecondThread;

#if(configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t semControlblock;
#endif
static vosSemDef_t vosSem_Handle;
vosSem sem;


UINT32 ulAppTicks ;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}

void vApplicationTickHook( void )
{   
    ulAppTicks++;
}

static void Main_Task(void const *parameter)
{   
    (void)parameter;

    BaseType_t xReturn = pdPASS;
        
    while (1)
    {
        if( Key_ScanNum(KEY3) == KEYSTATE_PRESSED )
        {
            if (NULL != sem)
            {
                xReturn = vosSemWait(sem, 0); //get immediately
                
                if (osOK == xReturn)
                    printf("key2 pressed, get 1 semphore count\r\n");
                else
                    printf("key2 pressed, can not get, 0x%x\r\n",xReturn);
                
                LED_On(LED1);
            }
        }
    }
}

static void Second_Task(void const *parameter)
{   
    BaseType_t xReturn = pdPASS;
    
    while (1)
    {
        if (Key_ScanNum(KEY1) == KEYSTATE_PRESSED)
        {
            printf("send message£¡\r\n");

            if (NULL != sem)
            {
                xReturn = vosSemRelease(sem);
                
                if(osOK == xReturn)
                    printf("give CountSem_Handle success!\r\n\r\n");
                else 
                    printf("no semaphore to give\r\n");
            }

            LED_Off(LED1);
        }
        
        vTaskDelay(20);
    }

}


void MainInit(void)
{
    //BeepInit();
    UartCmdInit();
    LED_BSP_Init();
    KeyBoardInit();

}

static  void  AppTaskRoot (void const *p_arg)
{
    (void)p_arg;
    
    MainInit();

    taskENTER_CRITICAL();

#if(configSUPPORT_STATIC_ALLOCATION == 1)
    memset(&semControlblock,0,sizeof(semControlblock));
    vosSem_Handle.controlblock = &semControlblock;
#endif
    vosSem_Handle.dummy = 5; //max value 5

    sem = vosSemCreate(&vosSem_Handle, 5); // init 5 sem count
    
    if (NULL != sem)
    {
        printf("create CountSem_Handle success!\r\n");
    }
    
    /* create Main_Task */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskMainControl,0,sizeof(AppTaskMainControl));
    vosMainThread.controlblock = &AppTaskMainControl;
    vosMainThread.buffer  = AppTaskMainStk;
#endif

    vosMainThread.name = "Main_Task";
    vosMainThread.pthread = Main_Task;
    vosMainThread.tpriority = APP_TASK_MAIN_PRIO;
    vosMainThread.stacksize = APP_TASK_MAIN_STK_SIZE;
    vosMainThread.handle = vosThreadCreate(&vosMainThread,NULL);

    if (NULL != vosMainThread.handle)
        printf("create Main_Task success!\r\n");
        
     /*create Second_Task */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskSecondControl,0,sizeof(AppTaskSecondControl));
    vosSecondThread.controlblock = &AppTaskSecondControl;
    vosSecondThread.buffer  = AppTaskSecondStk;
#endif
     vosSecondThread.name = "Second_Task";
     vosSecondThread.pthread = Second_Task;
     vosSecondThread.tpriority = APP_TASK_SECOND_PRIO;
     vosSecondThread.stacksize = APP_TASK_SECOND_STK_SIZE;
     vosSecondThread.handle = vosThreadCreate(&vosSecondThread,NULL);
     
     if (NULL != vosSecondThread.handle)
         printf("create Second_Task success!\r\n");

     vosThreadTerminate(vosRootThread.handle);
     
     taskEXIT_CRITICAL();


}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    HAL_Init();

    SystemClock_Config();
    
    BSP_Init();

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    memset(&AppTaskRootControl,0,sizeof(AppTaskRootControl));
    vosRootThread.controlblock = &AppTaskRootControl;
    vosRootThread.buffer = AppTaskRootStk;
#endif

    vosRootThread.name = "Root";
    vosRootThread.pthread = AppTaskRoot;
    vosRootThread.tpriority = tskIDLE_PRIORITY+1; // in freeRTOS, task priority is from 0 up to max_prio,prio 0 is the idle task
    vosRootThread.stacksize = APP_TASK_ROOT_STK_SIZE;
    vosRootThread.handle    = vosThreadCreate( &vosRootThread, NULL );

    if (vosRootThread.handle != NULL)
        vosKernelStart();
    

    while(1);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
