#ifndef _VOS_H_
#define _VOS_H_

#include "DtypeStm32.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef QueueHandle_t     VOS_QUEUE;

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


TaskHandle_t VOS_ThreadCreate (const vosThreadDef_t *thread_def, void *argument);

int VOS_ThreadTerminate (TaskHandle_t thread_id);

VOS_QUEUE VOS_CreateQueue(const vosQueueDef_t *queue_def ,unsigned int iSize);
int VOS_QueueP(VOS_QUEUE pQueue,uint32_t timeout, void *msg);
int VOS_QueueV(VOS_QUEUE pQueue,uint32_t timeout, void **msg);
int VOS_DestroyQueue(VOS_QUEUE pQueue);

#endif// _VOS_H_

