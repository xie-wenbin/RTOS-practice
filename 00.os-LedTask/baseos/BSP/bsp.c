/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS
//#include <includes.h>

#include "stm32f10x.h"

#include "stm32f10x_spi.h"

#include "bsp.h"

//#include "os_cfg.h"

//#include "memory.h"
//#include "task.h"

//#include "timer.h"

//#include "sys_time.h"


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/
static uint32_t fac_us=0;                                      
static uint32_t fac_ms=0;                           
    
#define OS_TICKS_PER_SEC 1000

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
//static volatile ErrorStatus HSEStartUpStatus = SUCCESS;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void SysTick_Configuration (void);

void SysTick_DelayMs(int ms);

void SysTick_DelayUs(int us);


//void GPIO_Config(void);
//void SPI_Config(void);
//void InterruptConfig(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         Global Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init(void)
{
    SystemInit();


    /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC 
         | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG 
         | RCC_APB2Periph_AFIO, ENABLE);


   //SysTick_Configuration();   
   /* Initialize the uC/OS-II tick interrupt */
  
   //InitMem();
  
   //VOS_TaskInit();
  
   //MessageQueueInit();
  
   //InitTimer();

   //sys_inittime();

}


/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
*
* Arguments   : None.
*
* Returns     : None.
*********************************************************************************************************
*/
void  BSP_IntDisAll (void)
{
  // CPU_IntDis();
}

/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         uC/OS-II Timer Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*
* Note(s)     : 1) The timer is setup for output compare mode BUT 'MUST' also 'freerun' so that the timer
*                  count goes from 0x00000000 to 0xFFFFFFFF to ALSO be able to read the free running count.
*                  The reason this is needed is because we use the free-running count in uC/OS-View.
*********************************************************************************************************
*/

void SysTick_Configuration(void)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC))
  { 
    /* Capture error */ 
    while (1);
  }

  fac_us = SystemCoreClock /1000000;
  fac_ms = 1000/OS_TICKS_PER_SEC;
  
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x1);
}

void SysTick_DelayUs(int us)
{
#if 0
  u32 ticks;
  u32 told,tnow,tcnt=0;
  u32 reload = SysTick->LOAD;                    
  ticks = us*fac_us;                      
  
  OSSchedLock();
  told = SysTick->VAL;                    
  while(1)
  {
      tnow = SysTick->VAL;    
      if(tnow != told)
      {       
          if(tnow < told) 
          {
              tcnt += told-tnow;        
          }
          else 
          {         
              tcnt += reload - tnow + told;  
          }
          told = tnow;
          if(tcnt >= ticks)
          {         
              break;                   
          }
      }  
  }
  OSSchedUnlock();
#endif
}

void SysTick_DelayMs(int ms)
{
#if 0
  if(ms >= fac_ms)                        
  { 
      OSTimeDly(ms /fac_ms);            
  }
  ms %= fac_ms;                         
  SysTick_DelayUs((u32)(ms*1000));   
#endif
}


