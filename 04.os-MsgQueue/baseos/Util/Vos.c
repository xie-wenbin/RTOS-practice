#include "vos.h"


#if( configSUPPORT_STATIC_ALLOCATION == 1 )

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleTaskStackBuffer[configMINIMAL_STACK_SIZE];

static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerTaskStackBuffer[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = xIdleTaskStackBuffer;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* If static allocation is supported then the application must provide the
    following callback function - which enables the application to optionally
    provide the memory that will be used by the timer task as the task's stack
    and TCB. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
    *ppxTimerTaskStackBuffer = xTimerTaskStackBuffer;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#endif


TaskHandle_t VOS_ThreadCreate (const vosThreadDef_t *thread_def, void *argument)
{
    BaseType_t xReturn = pdPASS;
    TaskHandle_t  handle;
    
    taskENTER_CRITICAL();
    
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    handle = xTaskCreateStatic((TaskFunction_t )thread_def->pthread,
                                (const char*    )thread_def->name,
                                thread_def->stacksize,
                                argument,
                                thread_def->tpriority,
                                thread_def->pBuffer,
                                thread_def->pTaskTcb);

    
#else
    xReturn = xTaskCreate((TaskFunction_t )thread_def->pthread,
                          (const char*    )thread_def->name,
                          (uint16_t )thread_def->stacksize,
                          argument,
                          thread_def->tpriority,
                          &handle);
    
#endif

    taskEXIT_CRITICAL();

    if (pdPASS == xReturn)
        return handle;
    else
        return NULL;
}

int VOS_ThreadTerminate (TaskHandle_t thread_id)
{
    if (NULL != thread_id)
        vTaskDelete(thread_id);

    return 0;

}


VOS_QUEUE VOS_CreateQueue(const vosQueueDef_t *queue_def ,unsigned int iSize)
{    
    VOS_QUEUE handle;
    
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    handle = xQueueCreateStatic((UBaseType_t ) queue_def->length,
                                (UBaseType_t ) iSize,
                                queue_def->pucQueueStorage,
                                queue_def->pStaticQueue);
#else
    handle = xQueueCreate((UBaseType_t ) queue_def->length,(UBaseType_t ) iSize);
#endif

    return handle;
}

int VOS_QueueP(VOS_QUEUE pQueue,uint32_t timeout, void *msg)
{
    BaseType_t xReturn = pdFAIL;
    BaseType_t xTaskWokenByReceive=pdFALSE;    

    if (pQueue != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            xReturn = xQueueSendFromISR(pQueue,msg,&xTaskWokenByReceive );  
            
            portYIELD_FROM_ISR(xTaskWokenByReceive);
        }
        else
        {
            xReturn = xQueueSend(pQueue,msg,timeout );  
        }
    }

    return xReturn;
}

int VOS_QueueV(VOS_QUEUE pQueue,uint32_t timeout, void **msg)
{
    BaseType_t xReturn = pdFAIL;
    BaseType_t xTaskWokenByReceive=pdFALSE;    
    
    if (pQueue != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            xReturn = xQueueReceiveFromISR(pQueue,*msg,&xTaskWokenByReceive );  
            
            portYIELD_FROM_ISR(xTaskWokenByReceive);
        }
        else
        {
            xReturn = xQueueReceive(pQueue,*msg, timeout);;  
        }
    }

    return xReturn;
}

int VOS_DestroyQueue(VOS_QUEUE pQueue)
{
    if (pQueue != NULL)
        vQueueDelete(pQueue);
    
    return 0;
}


