#ifndef _VOS_H_
#define _VOS_H_
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "DTypeStm32.h"

#ifdef  __cplusplus
extern "C"
{
#endif
    
#define osWaitForever     0xFFFFFFFF     ///< wait forever timeout value

typedef enum  {
  osOK                    =     0,       ///< function completed; no error or event occurred.
  osEventSignal           =  0x08,       ///< function completed; signal event occurred.
  osEventMessage          =  0x10,       ///< function completed; message event occurred.
  osEventMail             =  0x20,       ///< function completed; mail event occurred.
  osEventTimeout          =  0x40,       ///< function completed; timeout occurred.
  osErrorParameter        =  0x80,       ///< parameter error: a mandatory parameter was missing or specified an incorrect object.
  osErrorResource         =  0x81,       ///< resource not available: a specified resource was not available.
  osErrorTimeoutResource  =  0xC1,       ///< resource not available within given time: a specified resource was not available within the timeout period.
  osErrorISR              =  0x82,       ///< not allowed in ISR context: the function cannot be called from interrupt service routines.
  osErrorISRRecursive     =  0x83,       ///< function called multiple times from ISR with same object.
  osErrorPriority         =  0x84,       ///< system cannot determine priority or thread has illegal priority.
  osErrorNoMemory         =  0x85,       ///< system is out of memory: it was impossible to allocate or reserve memory for the operation.
  osErrorValue            =  0x86,       ///< value of a parameter is out of range.
  osErrorOS               =  0xFF,       ///< unspecified RTOS error: run-time error but no other error message fits.
  os_status_reserved      =  0x7FFFFFFF  ///< prevent from enum down-size compiler optimization.
} osStatus;


/// Timer type value for the timer definition.
typedef enum  {
  osTimerOnce             =     0,       ///< one-shot timer
  osTimerPeriodic         =     1        ///< repeating timer
} os_timer_type;


typedef void (*vos_pthread) (void const *argument);                   // Entry point of a thread.
typedef void (*vos_ptimer)  (void const *argument);                    // Entry point of a timer call back function.

typedef TaskHandle_t       vosThreadId;          // Thread ID identifies the thread (pointer to a thread control block).
typedef TimerHandle_t      vosTimerId;           // Timer ID identifies the timer (pointer to a timer control block).
typedef SemaphoreHandle_t  vosMutex;             // Mutex ID identifies the mutex (pointer to a mutex control block). 
typedef SemaphoreHandle_t  vosSem;               // Semaphore ID identifies the semaphore (pointer to a semaphore control block).
typedef struct os_pool_cb  *vosPoolId;           // Pool ID identifies the memory pool (pointer to a memory pool control block).
typedef QueueHandle_t      vosMsgQueue;          // Message ID identifies the message queue (pointer to a message queue control block).
typedef struct os_mailQ_cb *vosMailQId;          // Mail ID identifies the mail queue (pointer to a mail queue control block).



/**
  ******************************************************************************
  * cortex M3 define Interrupt Register
  * INTERRUPT CONTROL REG: 0xE000_ED04
  *
  ******************************************************************************
  */
#define VOS_CORTEX_NVIC_INT_CTRL         ( (volatile uint32_t *) 0xe000ed04 )

#define VOS_CORTEX_NVIC_VECTACTIVE       0x000001FFU      /* for cortex m3. */

#define VOS_Is_Irq_Context()           ((*(VOS_CORTEX_NVIC_INT_CTRL) & VOS_CORTEX_NVIC_VECTACTIVE))

#define VOS_WaitForever                 0xFFFFFFFFUL     ///< wait forever timeout value



typedef struct os_thread_def  {
  char                   *name;        ///< Thread name 
  vos_pthread             pthread;      ///< start address of thread function
  UBaseType_t            tpriority;    ///< initial thread priority
  uint32_t               instances;    ///< maximum number of instances of that thread function
  uint32_t               stacksize;    ///< stack size requirements in bytes; 0 is default stack size
  vosThreadId            handle;
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  uint32_t               *buffer;      ///< stack buffer for static allocation; NULL for dynamic allocation
  StaticTask_t           *controlblock;     ///< control block to hold thread's data for static allocation; NULL for dynamic allocation
#endif
} vosThreadDef_t;


typedef struct os_timer_def  {
  vos_ptimer                 ptimer;    ///< start address of a timer function
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticTimer_t            *controlblock;      ///< control block to hold timer's data for static allocation; NULL for dynamic allocation
#endif
} vosTimerDef_t;

typedef struct os_mutex_def  {
  uint32_t                   dummy;    ///< dummy value.
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticSemaphore_t         *controlblock;      ///< control block for static allocation; NULL for dynamic allocation
#endif
} vosMutexDef_t;

typedef struct os_semaphore_def  {
  uint32_t                   dummy;    ///< dummy value.
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticSemaphore_t        *controlblock;      ///< control block for static allocation; NULL for dynamic allocation
#endif
} vosSemDef_t;

typedef struct os_messageQ_def  {
  uint32_t                queue_sz;    ///< number of elements in the queue
  uint32_t                item_sz;    ///< size of an item
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  uint8_t                 *buffer;      ///< buffer for static allocation; NULL for dynamic allocation
  StaticQueue_t           *controlblock;     ///< control block to hold queue's data for static allocation; NULL for dynamic allocation
#endif
  //void                       *pool;    ///< memory array for messages
} vosMessageQDef_t;

typedef struct os_mailQ_def  {
  uint32_t                queue_sz;    ///< number of elements in the queue
  uint32_t                 item_sz;    ///< size of an item
  struct os_mailQ_cb **cb;
} vosMailQDef_t;

typedef struct os_pool_def  {
  uint32_t                 pool_sz;    ///< number of items (elements) in the pool
  uint32_t                 item_sz;    ///< size of an item
  void                       *pool;    ///< pointer to memory for pool
} vosPoolDef_t;

typedef struct  {
  int                 status;     ///< status code: event or error information
  union  {
    uint32_t                    v;     ///< message as 32-bit value
    void                       *p;     ///< message or mail as void pointer
    int32_t               signals;     ///< signal flags
  } value;                             ///< event value
  union  {
    vosMailQId             mail_id;     ///< mail id obtained by \ref osMailCreate
    vosMsgQueue       message_id;     ///< message id obtained by \ref osMessageCreate
  } def;                               ///< event definition
} vosEvent;


int vosKernelStart (void);

//  ==== osThread ====
vosThreadId vosThreadCreate (const vosThreadDef_t *thread_def, void *argument);
vosThreadId vosThreadGetId (void);
int vosThreadTerminate (vosThreadId thread_id);
int vosThreadYield (void);
int vosThreadSetPriority (vosThreadId thread_id, UBaseType_t priority);
UBaseType_t vosThreadGetPriority (vosThreadId thread_id);
int vosDelay (uint32_t millisec);

//  ==== Timer ====
vosTimerId vosTimerCreate (const vosTimerDef_t *timer_def, os_timer_type type, void *argument);
int vosTimerStart (vosTimerId timer_id, uint32_t millisec);
int vosTimerStop (vosTimerId timer_id);
int vosTimerDelete (vosTimerId timer_id);

//  ==== Signal Management ====
int32_t vosSignalSet (vosThreadId thread_id, int32_t signals);
int32_t vosSignalClear (vosThreadId thread_id, int32_t signals);
vosEvent vosSignalWait (int32_t signals, uint32_t millisec);

//  ==== memory pool ====
vosPoolId vosPoolCreate (const vosPoolDef_t *pool_def);
void *vosPoolAlloc (vosPoolId pool_id);
void *vosPoolCAlloc (vosPoolId pool_id);
int vosPoolFree (vosPoolId pool_id, void *block);

//  ==== Mutex ====
vosMutex vosMutexCreate (const vosMutexDef_t *mutex_def);
int vosMutexWait (vosMutex mutex_id, uint32_t millisec);
int vosMutexRelease (vosMutex mutex_id);
int vosMutexDelete (vosMutex mutex_id);

//  ==== Sem ====
vosSem vosSemCreate (const vosSemDef_t *semaphore_def, int32_t count);
int32_t vosSemWait (vosSem semaphore_id, uint32_t millisec);
int vosSemRelease (vosSem semaphore_id);
int vosSemDelete (vosSem semaphore_id);

//  ==== MessageQueue ====
vosMsgQueue vosMessageCreate (const vosMessageQDef_t *queue_def, vosThreadId thread_id);
int vosMessagePut (vosMsgQueue queue_id, uint32_t info, uint32_t millisec);
vosEvent vosMessageGet (vosMsgQueue queue_id, uint32_t millisec);
vosEvent vosMessagePeek (vosMsgQueue queue_id, uint32_t millisec);
uint32_t vosMessageWaiting(vosMsgQueue queue_id);
uint32_t vosMessageAvailableSpace(vosMsgQueue queue_id);
int vosMessageDelete (vosMsgQueue queue_id);

//  ==== MailBox ====
vosMailQId vosMailCreate (const vosMailQDef_t *queue_def, vosThreadId thread_id);
void *vosMailAlloc (vosMailQId queue_id, uint32_t millisec);                          // Allocate a memory block from a mail.
void *vosMailCAlloc (vosMailQId queue_id, uint32_t millisec);                         // Allocate a memory block from a mail and set memory block to zero.
int vosMailPut (vosMailQId queue_id, void *mail);
vosEvent vosMailGet (vosMailQId queue_id, uint32_t millisec);
int vosMailFree (vosMailQId queue_id, void *mail);


/*************************** Additional specific APIs to Free RTOS ************/
/**
* @brief  Handles the tick increment
* @param  none.
* @retval none.
*/
void vosSystickHandler(void);
int vosThreadSuspend (vosThreadId thread_id);
int vosThreadResume (vosThreadId thread_id);
int vosThreadSuspendAll (void);
int vosThreadResumeAll (void);
int vosDelayUntil (uint32_t *PreviousWakeTime, uint32_t millisec);
int vosAbortDelay(vosThreadId thread_id);
int vosThreadList (uint8_t *buffer);
vosMutex vosRecursiveMutexCreate (const vosMutexDef_t *mutex_def);
int vosRecursiveMutexRelease (vosMutex mutex_id);
int vosRecursiveMutexWait (vosMutex mutex_id, uint32_t millisec);
uint32_t vosSemaphoreGetCount(vosSem semaphore_id);



extern void  OS_CPU_SR_Restore(uint32_t ulMask);
extern uint32_t  OS_CPU_SR_Save(void);

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}  //disable interrupts
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}  //enable interrupts


#ifdef  __cplusplus
}
#endif

#endif //_VOS_H_
