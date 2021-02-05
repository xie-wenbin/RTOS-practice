#include "vos.h"


void  OS_CPU_SR_Restore(uint32_t ulMask);
uint32_t  OS_CPU_SR_Save(void);

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
    
    thread_id = NULL;

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

int VOS_QueueSend(VOS_QUEUE pQueue,uint32_t timeout, void *msg)
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

int VOS_QueueGet(VOS_QUEUE pQueue,uint32_t timeout, void **msg)
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
    
    pQueue = NULL;
    return 0;
}
VOS_SEM VOS_CreateSem(vosSemDef_t *sem_def,unsigned int value)
{
    VOS_SEM Sem = NULL;
    
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    Sem = xSemaphoreCreateCountingStatic((UBaseType_t )sem_def->dummy,value,sem_def->pSemaphoreBuffer);
#else
    Sem = xSemaphoreCreateCounting(sem_def->dummy,value);
#endif

    return Sem;
}


VOS_SEM VOS_CreateBinarySem(vosSemDef_t* binarySem_t)
{
    VOS_SEM BinarySem = NULL;

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    BinarySem = xSemaphoreCreateBinaryStatic(binarySem_t->pSemaphoreBuffer);
#else
    BinarySem = xSemaphoreCreateBinary();
#endif

    return BinarySem;
}

int VOS_SemGet(VOS_SEM pSem,uint32_t ms)
{
    BaseType_t xReturn = pdFAIL;
    BaseType_t xTaskWokenByReceive=pdFALSE;    

    if (pSem != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            xReturn = xSemaphoreTakeFromISR(pSem,&xTaskWokenByReceive );  
            
            portYIELD_FROM_ISR(xTaskWokenByReceive);
        }
        else
        {
            xReturn = xSemaphoreTake(pSem,ms);  
        }
    }

    return xReturn;

}

int VOS_SemV(VOS_SEM pSem)
{
    BaseType_t xReturn = pdFAIL;
    BaseType_t xTaskWokenByReceive=pdFALSE;    

    if (pSem != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            xReturn = xSemaphoreTakeFromISR(pSem,&xTaskWokenByReceive );  
            
            portYIELD_FROM_ISR(xTaskWokenByReceive);
        }
        else
        {
            xReturn = xSemaphoreTake(pSem,portMAX_DELAY);  
        }
    }

    return xReturn;

}


int VOS_SemSend(VOS_SEM pSem)
{
    BaseType_t xReturn = pdFAIL;
    BaseType_t xTaskWokenByReceive=pdFALSE;    

    if (pSem != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            xReturn = xSemaphoreGiveFromISR(pSem,&xTaskWokenByReceive );  
            
            portYIELD_FROM_ISR(xTaskWokenByReceive);
        }
        else
        {
            xReturn = xSemaphoreGive(pSem);  
        }
    }

    return xReturn;
}

void VOS_DestroySem(VOS_SEM *pSem)
{
    if (pSem != NULL)
    {
        vSemaphoreDelete(*pSem);
        pSem = NULL;
    }
}


VOS_MUTEX VOS_MutexCreate (const vosMutexDef_t *mutex_def)
{
    VOS_MUTEX mutex = NULL;

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    mutex = xSemaphoreCreateMutexStatic(mutex_def->controlblock);
#else
    mutex = xSemaphoreCreateMutex();
#endif

    return mutex;
}

int VOS_MutexGet (VOS_MUTEX mutex_id, uint32_t ms)
{
    BaseType_t xReturn = pdFAIL;

    if (mutex_id != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            //mutex can not use in ISR context
        }
        else
        {
            xReturn = xSemaphoreTake(mutex_id,ms);  
        }
    }

    return xReturn;
}

int VOS_MutexSend(VOS_MUTEX mutex_id)
{
    BaseType_t xReturn = pdFAIL;

    if (mutex_id != NULL)
    {
        if (VOS_Is_Irq_Context())
        {
            //mutex can not use in ISR context
        }
        else
        {
            xReturn = xSemaphoreGive(mutex_id);  
        }
    }

    return xReturn;
}

void VOS_MutexDelete (VOS_MUTEX mutex_id)
{
    if (mutex_id != NULL)
    {
        vSemaphoreDelete(mutex_id);
        mutex_id = NULL;
    }
}

uint32_t  OS_CPU_SR_Save(void)
{
    if (VOS_Is_Irq_Context())
    {
        return portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else
    {
        portDISABLE_INTERRUPTS();
    }
    
    return 0;
}

void  OS_CPU_SR_Restore(uint32_t ulMask)
{
    if (VOS_Is_Irq_Context())
    {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(ulMask);
    }
    else
    {
        portENABLE_INTERRUPTS();
    }
}

