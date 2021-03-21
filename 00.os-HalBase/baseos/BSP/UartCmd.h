 /**
*****************************************************************
* @file: xx.h
* @author: wen  
* @date:  12/3/2020
* @brief:  
* @attention:
****************************************************************
 */
#ifndef _UART_CMD_H_
#define _UART_CMD_H_

#include "stm32f1xx.h"
#include "DTypeStm32.h"
#include <stdio.h>

#define USART_REC_LEN  			50  	
#define EN_USART1_RX 			1	
#define RXBUFFERSIZE   1 					//�����С
extern u8 aRxBuffer[RXBUFFERSIZE];			//HAL��USART����Buffer
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; 
extern u16 USART_RX_STA;         		

void UartCmdInit(void);




#endif /* _UART_CMD_H_ */
