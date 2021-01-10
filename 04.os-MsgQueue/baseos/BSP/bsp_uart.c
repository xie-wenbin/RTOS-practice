#include "bsp_uart.h"
#include "string.h"

#include "vos.h" 



#define BSP_COM1_STR                    "USART1"
#define BSP_COM1                        USART1
#define BSP_COM1_CLK                    RCC_APB2Periph_USART1
#define BSP_COM1_TX_PIN                 GPIO_Pin_9
#define BSP_COM1_TX_GPIO_PORT           GPIOA
#define BSP_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BSP_COM1_RX_PIN                 GPIO_Pin_10
#define BSP_COM1_RX_GPIO_PORT           GPIOA
#define BSP_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BSP_COM1_IRQn                   USART1_IRQn

#define BSP_COM1_USART_BAUDRATE         115200

#if 1
#pragma import(__use_no_semihosting)             
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
void _sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#else
int fputc(int ch, FILE *f)
{
        USART_SendData(BSP_COM1, (uint8_t) ch);
        
        while (USART_GetFlagStatus(BSP_COM1, USART_FLAG_TXE) == RESET);     
    
        return (ch);
}

int fgetc(FILE *f)
{
        while (USART_GetFlagStatus(BSP_COM1, USART_FLAG_RXNE) == RESET);

        return (int)USART_ReceiveData(BSP_COM1);
}
#endif 


u8 USART_RX_BUF[USART_REC_LEN];

//bit15,  complete
//bit14,  receive 0x0d
//bit13~0, receive length
u16 USART_RX_STA=0;      


void UartCmdInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(BSP_COM1_TX_GPIO_CLK|BSP_COM1_RX_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(BSP_COM1_CLK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = BSP_COM1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BSP_COM1_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BSP_COM1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BSP_COM1_RX_GPIO_PORT, &GPIO_InitStructure);
    

    USART_InitStructure.USART_BaudRate = BSP_COM1_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(BSP_COM1, &USART_InitStructure);
    
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = BSP_COM1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(BSP_COM1,USART_IT_RXNE, ENABLE); 
    
    USART_Cmd(BSP_COM1, ENABLE);        
}

extern VOS_QUEUE Uart_Queue;

void USART1_IRQHandler(void)                
{
	u8 Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //must end with 0x0d 0x0a
	{
		Res =USART_ReceiveData(USART1);	
		
		if((USART_RX_STA&0x8000)==0)//receive not complete
			{
			if(USART_RX_STA&0x4000)
				{
				if(Res!=0x0a)USART_RX_STA=0;
				else USART_RX_STA|=0x8000;
				}
			else //not receive 0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;
					}		 
				}
			}   		 
     } 
	
	if((USART_RX_STA&0x8000)&&(Uart_Queue!=NULL))
	{
		VOS_QueueP(Uart_Queue,0,USART_RX_BUF);
		
		USART_RX_STA=0;	

        memset(USART_RX_BUF,0,USART_REC_LEN);
	
	}
} 

