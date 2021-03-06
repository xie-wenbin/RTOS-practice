#include "string.h"
#include "uartTimer.h"


u8 buffer[USART_REC_LEN];

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
    
    TIM_TimeBaseStructure.TIM_Period = arr;             
    TIM_TimeBaseStructure.TIM_Prescaler =psc;           
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     
 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );       

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
    NVIC_Init(&NVIC_InitStructure);                 

    TIM_Cmd(TIM2, ENABLE);                              
}


extern VOS_QUEUE Uart_Queue;    

void TIM2_IRQHandler(void)
{
    BaseType_t err;
    
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
    {
        if(Uart_Queue!=NULL)
        {
            void * pBuff = &buffer;
            memset(buffer,0,USART_REC_LEN); 
            
            err=VOS_QueueV(Uart_Queue,0,&pBuff);//Message_Queue

            if(err==pdTRUE)         
            {
                printf("%s",buffer);    
            }
        }
        
    }
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
}











