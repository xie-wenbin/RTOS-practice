/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS

#include "stm32f1xx_hal.h"

#include "bsp.h"
//#include "memory.h"
//#include "vtask.h"
//#include "timer.h"

#define DELAYCLOCK_1us		SystemCoreClock/1000000
#define DELAYCLOCK_10us		SystemCoreClock/100000
#define DELAYCLOCK_100us	SystemCoreClock/10000
#define DELAYCLOCK_1ms		SystemCoreClock/1000

static uint32_t fac_us=0;                                      
static uint32_t fac_ms=0;                           


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void SysTick_Configuration (void);

void SysTick_DelayMs(int ms);

void SysTick_DelayUs(int us);



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
    SysTick_Configuration();
    
    /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */

   //InitMem();
  
   //VOS_TaskInit();
  
   //MessageQueueInit();

   //InitTimer();

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

void SysTick_Configuration(void)
{
#if (USE_SUPPORT_OS > 0)
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (HAL_SYSTICK_Config(SystemCoreClock / OS_TICKS_PER_SEC))
  { 
    /* Capture error */ 
    while (1);
  }

  fac_us = SystemCoreClock /1000000;
  fac_ms = 1000/OS_TICKS_PER_SEC;
  
  /* Configure the SysTick handler priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
#else
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  fac_us = SystemCoreClock /1000000;

#endif
}


#if (USE_SUPPORT_OS > 0)
void SysTick_DelayUs(int us)
{
  u32 ticks;
  u32 told,tnow,tcnt=0;
  u32 reload = SysTick->LOAD;                    
  ticks = us*fac_us;   
  
  taskENTER_CRITICAL();
  
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
  
  taskEXIT_CRITICAL();
}

void SysTick_DelayMs(int ms)
{
  if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
  {       
   if(ms>=fac_ms)                  
   { 
       vTaskDelay(ms/fac_ms);        
   }
   ms%=fac_ms;
  }
  SysTick_DelayUs((u32)(ms*1000));   
}
#else

void SysTick_DelayUs(int us)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD; 
	ticks=us*fac_us; 
	
	told=SysTick->VAL;
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow<told)
            {
                tcnt += told - tnow;
            }
			else 
            {
                tcnt += reload - tnow + told;
            }
			told=tnow;
			if(tcnt>=ticks)
                break;
		}  
	};
}

void SysTick_DelayMs(int nms)
{
	u32 i;
	for(i=0;i<nms;i++) SysTick_DelayUs(1000);
}
#endif
//delay ms,do not cause task scheduling
void delay_xms(u32 nms)
{
    u32 i;
	for(i=0;i<nms;i++) SysTick_DelayUs(1000);
}
