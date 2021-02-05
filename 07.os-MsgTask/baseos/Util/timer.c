#include "timer.h"
#include "vtask.h"
#include <stdio.h>


extern UINT32 ulAppTicks ;

TIMER TimerArray[MAX_TIMER_LEN];
UINT8 TimerIndex[MAX_TIMER_LEN];

void InitTimer(void)
{
    UINT8 loop;
    for(loop = 0; loop < MAX_TIMER_LEN;loop++)
	{
	    TimerIndex[loop] = 0;
        
	}
}

UINT8 FreeTimerSlot(UINT8 ucTimer)
{
    TimerIndex[ucTimer] = 0;
	return 0;
}


void RmvTimer(UINT8 TimerId)
{
    uint32_t     cpu_sr = 0;
    OS_ENTER_CRITICAL();
    if (TimerId < MAX_TIMER_LEN)
    {
        TimerIndex[TimerId] = 0;
    }
    OS_EXIT_CRITICAL();
}

UINT8 CheckTimer(UINT8 TimerId)
{
    if (TimerId >= MAX_TIMER_LEN)
    {
        return 0;
    }
    
    if (!TimerIndex[TimerId])
    {
        return 0;
    }


	return 1;
}
void ModTimer(UINT8 TimerId,UINT16 TimerLen,UINT16 usPara)
{

    uint32_t     cpu_sr = 0;

    if (TimerId >= MAX_TIMER_LEN)
    {
        return ;
    }
    if (!TimerIndex[TimerId])
    {
        return ;
    }

    OS_ENTER_CRITICAL();
    TimerArray[TimerId].TimerLen = TimerLen;
	  TimerArray[TimerId].dwTick = ulAppTicks;
    TimerArray[TimerId].Para = usPara;
    OS_EXIT_CRITICAL();


}

/*
     TimerLen: Unit of 100ms
    return value:
       0 succ
       other fail
*/
void AddTimer(UINT8 TimerId,UINT8 enType,UINT16 TimerLen,UINT16 Para)
{
     uint32_t     cpu_sr = 0;

	if (MAX_TIMER_LEN <= TimerId)
	{
		return ;
	}

    if (CheckTimer(TimerId))
    {
        ModTimer(TimerId,TimerLen,Para);
        return ;
    }
    
    OS_ENTER_CRITICAL();

    if (1 != TimerIndex[TimerId])
    {
        TimerIndex[TimerId] = 1;
    }
    TimerArray[TimerId].dwTick = ulAppTicks;
	TimerArray[TimerId].TimerId = TimerId;
	TimerArray[TimerId].TimerLen = TimerLen;
	TimerArray[TimerId].enType = enType;
	TimerArray[TimerId].Para = Para;
    OS_EXIT_CRITICAL();

}

void TimerProc(void)
{
     UINT8  loop;
     UINT32 curtick = ulAppTicks;
    
     
    // UART_PutChar('t');
     for(loop = 0; loop < MAX_TIMER_LEN;loop++)
     {   
         if (1 == TimerIndex[loop])
         {
             if ((TimerArray[loop].dwTick + TimerArray[loop].TimerLen) <= curtick)
             {
                 // fire timer message
                 Message * Msg = MessageAlloc(PID_TIMER,0);
                 //  UART_PutChar('T');
    
                 if (Msg)
                 {
                     Msg->msgHead.nMsgType = TimerArray[loop].TimerId;
                     Msg->msgHead.MsgSeq = TimerArray[loop].enType;
                     Msg->msgHead.AddData = TimerArray[loop].Para;
                     MessageSend(Msg);
                 }
    
                 if (OS_TMR_OPT_ONE_SHOT == TimerArray[loop].enType )
                 {
                     FreeTimerSlot(loop);
                 }
                 else
                 {
                    TimerArray[loop].dwTick = curtick;
                 }
             }
         }
     }


}


