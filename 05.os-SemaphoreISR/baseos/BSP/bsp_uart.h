 /**
*****************************************************************
* @file: xx.h
* @author: wen  
* @date:  12/3/2020
* @brief:  
* @attention:
****************************************************************
 */
#ifndef _BSP_UART_H__
#define _BSP_UART_H__

#include "stm32f10x.h"
#include <stdio.h>

#define USART_REC_LEN  			50  	
#define EN_USART1_RX 			1		
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; 
extern u16 USART_RX_STA;         		

void UartCmdInit(void);




#endif /* _BSP_UART_H__ */
