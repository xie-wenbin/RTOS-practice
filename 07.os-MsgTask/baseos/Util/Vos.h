#ifndef _VOS_H_
#define _VOS_H_

#include "DtypeStm32.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef QueueHandle_t     VOS_QUEUE;
typedef SemaphoreHandle_t VOS_SEM;
typedef SemaphoreHandle_t VOS_MUTEX;


typedef void (*vos_pthread) (void const *argument);

/* in "The Defi nitive Guide to the ARM Cortex-M3" , Chapter 8 NVIC */
#define CORTEX_NVIC_INTERRUPT_CTRL_REG     ( (volatile uint32_t *) 0xE000ED04 )

#define CORTEX_NVIC_VECTACTIVE             0x000001FFU      /* for cortex m3. which irq is active */

#define VOS_Is_Irq_Context()               ((*(CORTEX_NVIC_INTERRUPT_CTRL_REG) & CORTEX_NVIC_VECTACTIVE))


typedef struct vos_task_def {
    char                   *name;        ///< Thread name 
    vos_pthread             pthread;      ///< start address of thread function
    UBaseType_t             tpriority;    ///< initial thread priority
    uint32_t                stacksize;    ///< stack size requirements in bytes; 0 is default stack size
    TaskHandle_t            handle;
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    StackType_t             *pBuffer;   ///< stack buffer for static allocation; NULL for dynamic allocation
    StaticTask_t            *pTaskTcb;  ///< control block to hold thread's data for static allocation; NULL for dynamic allocation
#endif
}vosThreadDef_t;


typedef struct vos_queue_def  {
  uint32_t               length;    /// number of queues
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  uint8_t               *pucQueueStorage;
  StaticQueue_t         *pStaticQueue;      ///< control block for static allocation; NULL for dynamic allocation
#endif
} vosQueueDef_t;

typedef struct vos_semaphore_def  {
  uint32_t               dummy;    /// dummy values
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticSemaphore_t      *pSemaphoreBuffer;      ///< control block for static allocation; NULL for dynamic allocation
#endif
} vosSemDef_t;

typedef struct vos_mutex_def  {
  uint32_t                dummy;    ///< dummy value.
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticSemaphore_t       *controlblock;      ///< control block for static allocation; NULL for dynamic allocation
#endif
} vosMutexDef_t;



TaskHandle_t VOS_ThreadCreate (const vosThreadDef_t *thread_def, void *argument);

int VOS_ThreadTerminate (TaskHandle_t thread_id);

VOS_QUEUE VOS_CreateQueue(const vosQueueDef_t *queue_def ,unsigned int iSize);
int VOS_QueueSend(VOS_QUEUE pQueue,uint32_t timeout, void *msg);
int VOS_QueueGet(VOS_QUEUE pQueue,uint32_t timeout, void **msg);
int VOS_DestroyQueue(VOS_QUEUE pQueue);

VOS_SEM VOS_CreateSem(vosSemDef_t *sem_def,unsigned int value);
VOS_SEM VOS_CreateBinarySem(vosSemDef_t *binarySem_t);
int VOS_SemGet(VOS_SEM pSem,uint32_t ms); 
int VOS_SemV(VOS_SEM pSem);
int VOS_SemSend(VOS_SEM pSem); 
void VOS_DestroySem(VOS_SEM *pSem);

VOS_MUTEX VOS_MutexCreate (const vosMutexDef_t *mutex_def);
int VOS_MutexGet (VOS_MUTEX mutex_id, uint32_t ms);
int VOS_MutexSend(VOS_MUTEX mutex_id);
void VOS_MutexDelete (VOS_MUTEX mutex_id);


extern void  OS_CPU_SR_Restore(uint32_t ulMask);
extern uint32_t  OS_CPU_SR_Save(void);

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}  //disable interrupts
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}  //enable interrupts

#endif// _VOS_H_

