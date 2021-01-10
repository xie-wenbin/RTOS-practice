#include "bsp_uart.h"



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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(BSP_COM1,USART_IT_RXNE, ENABLE);	
	
	USART_Cmd(BSP_COM1, ENABLE);	    
}


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

