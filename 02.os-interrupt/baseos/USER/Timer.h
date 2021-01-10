#ifndef _TIMER_H_
#define _TIMER_H_

#include "DtypeStm32.h"
#include "stm32f10x.h"



void TIM3_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);


#endif// _TIMER_H_

