/**
  ******************************************************************************
  * File Name          : vos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 wen.
  * All rights reserved.</center></h2>
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "vos.h"

void  OS_CPU_SR_Restore(uint32_t ulMask);
uint32_t  OS_CPU_SR_Save(void);


#if   defined ( __CC_ARM )

  #define __ASM            __asm                                      
  #define __INLINE         __inline                                     
  #define __STATIC_INLINE  static __inline

  #include "cmsis_armcc.h"

/*
 * GNU Compiler
 */
#elif defined ( __GNUC__ )

  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  #define __STATIC_INLINE  static inline

  #include "cmsis_gcc.h"

/*
 * IAR Compiler
 */
#elif defined ( __ICCARM__ )

  #ifndef   __ASM
    #define __ASM                     __asm
  #endif
  #ifndef   __INLINE
    #define __INLINE                  inline
  #endif
  #ifndef   __STATIC_INLINE
    #define __STATIC_INLINE           static inline
  #endif

  #include <cmsis_iar.h>
#endif

extern void xPortSysTickHandler(void);


/* Determine whether we are in thread mode or handler mode. */
static int inHandlerMode (void)
{
  return __get_IPSR() != 0;
}

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerTaskStackBuffer[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
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



int vosKernelStart (void)
{
  vTaskStartScheduler();
  
  return osOK;
}

vosThreadId vosThreadCreate (const vosThreadDef_t *thread_def, void *argument)
{
  TaskHandle_t handle;
  
#if( configSUPPORT_STATIC_ALLOCATION == 1 ) &&  ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
  if((thread_def->buffer != NULL) && (thread_def->controlblock != NULL)) {
    handle = xTaskCreateStatic((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,
              thread_def->stacksize, argument, thread_def->tpriority,
              thread_def->buffer, thread_def->controlblock);
  }
  else {
    if (xTaskCreate((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,
              thread_def->stacksize, argument, thread_def->tpriority,
              &handle) != pdPASS)  {
      return NULL;
    } 
  }
#elif( configSUPPORT_STATIC_ALLOCATION == 1 )

    handle = xTaskCreateStatic((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,
              thread_def->stacksize, argument, thread_def->tpriority,
              thread_def->buffer, thread_def->controlblock);
#else
  if (xTaskCreate((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,
                   thread_def->stacksize, argument, thread_def->tpriority,
                   &handle) != pdPASS)  {
    return NULL;
  }     
#endif
  
  return handle;
}

vosThreadId vosThreadGetId (void)
{
#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )
  return xTaskGetCurrentTaskHandle();
#else
	return NULL;
#endif
}

int vosThreadTerminate (vosThreadId thread_id)
{
#if (INCLUDE_vTaskDelete == 1)
  vTaskDelete(thread_id);
  return osOK;
#else
  return osErrorOS;
#endif
}

int vosThreadYield (void)
{
  taskYIELD();
  
  return osOK;
}

int vosThreadSetPriority (vosThreadId thread_id, UBaseType_t priority)
{
#if (INCLUDE_vTaskPrioritySet == 1)
  vTaskPrioritySet(thread_id, priority);
  return osOK;
#else
  return osErrorOS;
#endif
}

UBaseType_t vosThreadGetPriority (vosThreadId thread_id)
{
#if (INCLUDE_uxTaskPriorityGet == 1)
  if (inHandlerMode())
  {
    return uxTaskPriorityGetFromISR(thread_id);  
  }
  else
  {  
    return uxTaskPriorityGet(thread_id);
  }
#else
  return osErrorOS;
#endif
}

int vosDelay (uint32_t millisec)
{
#if INCLUDE_vTaskDelay
  TickType_t ticks = millisec / portTICK_PERIOD_MS;
  
  vTaskDelay(ticks ? ticks : 1);          /* Minimum delay = 1 tick */
  
  return osOK;
#else
  (void) millisec;
  
  return osErrorResource;
#endif
}

vosTimerId vosTimerCreate (const vosTimerDef_t *timer_def, os_timer_type type, void *argument)
{
#if (configUSE_TIMERS == 1)

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) 
  if(timer_def->controlblock != NULL) {
    return xTimerCreateStatic((const char *)"",
                      1, // period should be filled when starting the Timer using osTimerStart
                      (type == osTimerPeriodic) ? pdTRUE : pdFALSE,
                      (void *) argument,
                      (TaskFunction_t)timer_def->ptimer,
                      (StaticTimer_t *)timer_def->controlblock);
  }
  else {
    return xTimerCreate((const char *)"",
                      1, // period should be filled when starting the Timer using osTimerStart
                      (type == osTimerPeriodic) ? pdTRUE : pdFALSE,
                      (void *) argument,
                      (TaskFunction_t)timer_def->ptimer);
 }
#elif( configSUPPORT_STATIC_ALLOCATION == 1 )
  return xTimerCreateStatic((const char *)"",
                      1, // period should be filled when starting the Timer using osTimerStart
                      (type == osTimerPeriodic) ? pdTRUE : pdFALSE,
                      (void *) argument,
                      (TaskFunction_t)timer_def->ptimer,
                      (StaticTimer_t *)timer_def->controlblock);  
#else
  return xTimerCreate((const char *)"",
                      1, // period should be filled when starting the Timer using osTimerStart
                      (type == osTimerPeriodic) ? pdTRUE : pdFALSE,
                      (void *) argument,
                      (TaskFunction_t)timer_def->ptimer);
#endif

#else 
	return NULL;
#endif
}

int vosTimerStart (vosTimerId timer_id, uint32_t millisec)
{
  int result = osOK;
#if (configUSE_TIMERS == 1)  
  portBASE_TYPE taskWoken = pdFALSE;
  TickType_t ticks = millisec / portTICK_PERIOD_MS;

  if (ticks == 0)
    ticks = 1;
    
  if (inHandlerMode()) 
  {
    if (xTimerChangePeriodFromISR(timer_id, ticks, &taskWoken) != pdPASS)
    {
      result = osErrorOS;
    }
    else
    {
      portEND_SWITCHING_ISR(taskWoken);     
    }
  }
  else 
  {
    if (xTimerChangePeriod(timer_id, ticks, 0) != pdPASS)
      result = osErrorOS;
  }

#else 
  result = osErrorOS;
#endif
  return result;
}

int vosTimerStop (vosTimerId timer_id)
{
  int result = osOK;
#if (configUSE_TIMERS == 1)  
  portBASE_TYPE taskWoken = pdFALSE;

  if (inHandlerMode()) {
    if (xTimerStopFromISR(timer_id, &taskWoken) != pdPASS) {
      return osErrorOS;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xTimerStop(timer_id, 0) != pdPASS) {
      result = osErrorOS;
    }
  }
#else 
  result = osErrorOS;
#endif 
  return result;
}

int vosTimerDelete (vosTimerId timer_id)
{
int result = osOK;

#if (configUSE_TIMERS == 1)

   if (inHandlerMode()) {
     return osErrorISR;
  }
  else { 
    if ((xTimerDelete(timer_id, osWaitForever )) != pdPASS) {
      result = osErrorOS;
    }
  } 
    
#else 
  result = osErrorOS;
#endif 
 
  return result;
}

int32_t vosSignalSet (vosThreadId thread_id, int32_t signal)
{
#if( configUSE_TASK_NOTIFICATIONS == 1 )	
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  uint32_t ulPreviousNotificationValue = 0;
  
  if (inHandlerMode())
  {
    if(xTaskGenericNotifyFromISR( thread_id , (uint32_t)signal, eSetBits, &ulPreviousNotificationValue, &xHigherPriorityTaskWoken ) != pdPASS )
      return 0x80000000;
    
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }  
  else if(xTaskGenericNotify( thread_id , (uint32_t)signal, eSetBits, &ulPreviousNotificationValue) != pdPASS )
    return 0x80000000;
  
  return ulPreviousNotificationValue;
#else
  (void) thread_id;
  (void) signal;

  return 0x80000000; /* Task Notification not supported */ 	
#endif
}

int32_t vosSignalClear (vosThreadId thread_id, int32_t signal);

vosEvent vosSignalWait (int32_t signals, uint32_t millisec)
{
  vosEvent ret;

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	
  TickType_t ticks;

  ret.value.signals = 0;  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }  
  
  if (inHandlerMode())
  {
    ret.status = osErrorISR;  /*Not allowed in ISR*/
  }
  else
  {
    if(xTaskNotifyWait( 0,(uint32_t) signals, (uint32_t *)&ret.value.signals, ticks) != pdTRUE)
    {
      if(ticks == 0)  ret.status = osOK;
      else  ret.status = osEventTimeout;
    }
    else if(ret.value.signals < 0)
    {
      ret.status =  osErrorValue;     
    }
    else  ret.status =  osEventSignal;
  }
#else
  (void) signals;
  (void) millisec;
	
  ret.status =  osErrorOS;	/* Task Notification not supported */
#endif
  
  return ret;
}


/**
  ************************************************
  *   osMemoryPool
  ************************************************
  */
typedef struct os_pool_cb {
  void *pool;
  uint8_t *markers;
  uint32_t pool_sz;
  uint32_t item_sz;
  uint32_t currentIndex;
} os_pool_cb_t;

/**
* @brief Create and Initialize a memory pool
* @param  pool_def      memory pool definition referenced with \ref osPool.
* @retval  memory pool ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osPoolCreate shall be consistent in every CMSIS-RTOS.
*/
vosPoolId vosPoolCreate (const vosPoolDef_t *pool_def)
{
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
  vosPoolId thePool;
  int itemSize = 4 * ((pool_def->item_sz + 3) / 4);
  uint32_t i;
  
  /* First have to allocate memory for the pool control block. */
 thePool = pvPortMalloc(sizeof(os_pool_cb_t));

  
  if (thePool) {
    thePool->pool_sz = pool_def->pool_sz;
    thePool->item_sz = itemSize;
    thePool->currentIndex = 0;
    
    /* Memory for markers */
    thePool->markers = pvPortMalloc(pool_def->pool_sz);
   
    if (thePool->markers) {
      /* Now allocate the pool itself. */
     thePool->pool = pvPortMalloc(pool_def->pool_sz * itemSize);
      
      if (thePool->pool) {
        for (i = 0; i < pool_def->pool_sz; i++) {
          thePool->markers[i] = 0;
        }
      }
      else {
        vPortFree(thePool->markers);
        vPortFree(thePool);
        thePool = NULL;
      }
    }
    else {
      vPortFree(thePool);
      thePool = NULL;
    }
  }

  return thePool;
 
#else
  return NULL;
#endif
}

/**
* @brief Allocate a memory block from a memory pool
* @param pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolAlloc shall be consistent in every CMSIS-RTOS.
*/
void *vosPoolAlloc (vosPoolId pool_id)
{
  int dummy = 0;
  void *p = NULL;
  uint32_t i;
  uint32_t index;
  
  if (inHandlerMode()) {
    dummy = portSET_INTERRUPT_MASK_FROM_ISR();
  }
  else {
    vPortEnterCritical();
  }
  
  for (i = 0; i < pool_id->pool_sz; i++) {
    index = (pool_id->currentIndex + i) % pool_id->pool_sz;
    
    if (pool_id->markers[index] == 0) {
      pool_id->markers[index] = 1;
      p = (void *)((uint32_t)(pool_id->pool) + (index * pool_id->item_sz));
      pool_id->currentIndex = index;
      break;
    }
  }
  
  if (inHandlerMode()) {
    portCLEAR_INTERRUPT_MASK_FROM_ISR(dummy);
  }
  else {
    vPortExitCritical();
  }
  
  return p;
}

/**
* @brief Allocate a memory block from a memory pool and set memory block to zero
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolCAlloc shall be consistent in every CMSIS-RTOS.
*/
void *vosPoolCAlloc (vosPoolId pool_id)
{
  void *p = vosPoolAlloc(pool_id);
  
  if (p != NULL)
  {
    memset(p, 0, sizeof(pool_id->pool_sz));
  }
  
  return p;
}

/**
* @brief Return an allocated memory block back to a specific memory pool
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @param  block         address of the allocated memory block that is returned to the memory pool.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osPoolFree shall be consistent in every CMSIS-RTOS.
*/
int vosPoolFree (vosPoolId pool_id, void *block)
{
  uint32_t index;
  
  if (pool_id == NULL) {
    return osErrorParameter;
  }
  
  if (block == NULL) {
    return osErrorParameter;
  }
  
  if (block < pool_id->pool) {
    return osErrorParameter;
  }
  
  index = (uint32_t)block - (uint32_t)(pool_id->pool);
  if (index % pool_id->item_sz) {
    return osErrorParameter;
  }
  index = index / pool_id->item_sz;
  if (index >= pool_id->pool_sz) {
    return osErrorParameter;
  }
  
  pool_id->markers[index] = 0;
  
  return osOK;
}

/**
  ************************************************
  *   osMutex
  ************************************************
  */
vosMutex vosMutexCreate (const vosMutexDef_t *mutex_def)
{
#if ( configUSE_MUTEXES == 1)

#if( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

  if (mutex_def->controlblock != NULL) {
    return xSemaphoreCreateMutexStatic( mutex_def->controlblock );
     }
  else {
    return xSemaphoreCreateMutex(); 
  }
#elif ( configSUPPORT_STATIC_ALLOCATION == 1 )
  return xSemaphoreCreateMutexStatic( mutex_def->controlblock );
#else  
    return xSemaphoreCreateMutex(); 
#endif
#else
  return NULL;
#endif
}

int vosMutexWait (vosMutex mutex_id, uint32_t millisec)
{
  TickType_t ticks;
  portBASE_TYPE taskWoken = pdFALSE;  
  
  
  if (mutex_id == NULL) {
    return osErrorParameter;
  }
  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }
  
  if (inHandlerMode()) {
    if (xSemaphoreTakeFromISR(mutex_id, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
	portEND_SWITCHING_ISR(taskWoken);
  } 
  else if (xSemaphoreTake(mutex_id, ticks) != pdTRUE) {
    return osErrorOS;
  }
  
  return osOK;
}

int vosMutexRelease (vosMutex mutex_id)
{
  int result = osOK;
  portBASE_TYPE taskWoken = pdFALSE;
  
  if (inHandlerMode()) {
    if (xSemaphoreGiveFromISR(mutex_id, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else if (xSemaphoreGive(mutex_id) != pdTRUE) 
  {
    result = osErrorOS;
  }
  return result;
}

int vosMutexDelete (vosMutex mutex_id)
{
  if (inHandlerMode()) {
    return osErrorISR;
  }

  vQueueDelete(mutex_id);

  return osOK;
}

/**
  ************************************************
  *   osSemaphore
  ************************************************
  */
vosSem vosSemCreate (const vosSemDef_t *semaphore_def, int32_t count)
{ 
#if( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

  vosSem sema;
  
  if (semaphore_def->controlblock != NULL){
    if (count == 1) {
      return xSemaphoreCreateBinaryStatic( semaphore_def->controlblock );
    }
    else {
#if (configUSE_COUNTING_SEMAPHORES == 1 )
      return xSemaphoreCreateCountingStatic( count, count, semaphore_def->controlblock );
#else
      return NULL;
#endif
    }
  }
  else {
    if (count == 1) {
      vSemaphoreCreateBinary(sema);
      return sema;
    }
    else {
#if (configUSE_COUNTING_SEMAPHORES == 1 )	
      return xSemaphoreCreateCounting(count, count);
#else
      return NULL;
#endif    
    }
  }
#elif ( configSUPPORT_STATIC_ALLOCATION == 1 ) // configSUPPORT_DYNAMIC_ALLOCATION == 0
  if(count == 1) {
    return xSemaphoreCreateBinaryStatic( semaphore_def->controlblock );
  }
  else
  {
#if (configUSE_COUNTING_SEMAPHORES == 1 )
      return xSemaphoreCreateCountingStatic( count, count, semaphore_def->controlblock );
#else
      return NULL;
#endif    
  }
#else  // configSUPPORT_STATIC_ALLOCATION == 0  && configSUPPORT_DYNAMIC_ALLOCATION == 1
  vosSem sema;
 
  if (count == 1) {
    vSemaphoreCreateBinary(sema);
    return sema;
  }
  else {
#if (configUSE_COUNTING_SEMAPHORES == 1 )	
    return xSemaphoreCreateCounting(count, count);
#else
    return NULL;
#endif
  }
#endif
}

int32_t vosSemWait (vosSem semaphore_id, uint32_t millisec)
{
  TickType_t ticks;
  portBASE_TYPE taskWoken = pdFALSE;  
  
  
  if (semaphore_id == NULL) {
    return osErrorParameter;
  }
  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }
  
  if (inHandlerMode()) {
    if (xSemaphoreTakeFromISR(semaphore_id, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
	portEND_SWITCHING_ISR(taskWoken);
  }  
  else if (xSemaphoreTake(semaphore_id, ticks) != pdTRUE) {
    return osErrorOS;
  }
  
  return osOK;
}

int vosSemRelease (vosSem semaphore_id)
{
  int result = osOK;
  portBASE_TYPE taskWoken = pdFALSE;
  
  
  if (inHandlerMode()) {
    if (xSemaphoreGiveFromISR(semaphore_id, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xSemaphoreGive(semaphore_id) != pdTRUE) {
      result = osErrorOS;
    }
  }
  
  return result;
}

int vosSemDelete (vosSem semaphore_id)
{
  if (inHandlerMode()) {
    return osErrorISR;
  }

  vSemaphoreDelete(semaphore_id);

  return osOK; 
}


/**
  ************************************************
  *   osMessageQueue
  ************************************************
  */
vosMsgQueue vosMessageCreate (const vosMessageQDef_t *queue_def, vosThreadId thread_id)
{
  (void) thread_id;
  
#if( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

  if ((queue_def->buffer != NULL) && (queue_def->controlblock != NULL)) {
    return xQueueCreateStatic(queue_def->queue_sz, queue_def->item_sz, queue_def->buffer, queue_def->controlblock);
  }
  else {
    return xQueueCreate(queue_def->queue_sz, queue_def->item_sz);
  }
#elif ( configSUPPORT_STATIC_ALLOCATION == 1 )
  return xQueueCreateStatic(queue_def->queue_sz, queue_def->item_sz, queue_def->buffer, queue_def->controlblock);
#else  
  return xQueueCreate(queue_def->queue_sz, queue_def->item_sz);
#endif
}

int vosMessagePut (vosMsgQueue queue_id, uint32_t info, uint32_t millisec)
{
  portBASE_TYPE taskWoken = pdFALSE;
  TickType_t ticks;
  
  ticks = millisec / portTICK_PERIOD_MS;
  if (ticks == 0) {
    ticks = 1;
  }
  
  if (inHandlerMode()) {
    if (xQueueSendFromISR(queue_id, &info, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xQueueSend(queue_id, &info, ticks) != pdTRUE) {
      return osErrorOS;
    }
  }
  
  return osOK;
}

vosEvent vosMessageGet (vosMsgQueue queue_id, uint32_t millisec)
{
  portBASE_TYPE taskWoken;
  TickType_t ticks;
  vosEvent event;
  
  event.def.message_id = queue_id;
  event.value.v = 0;
  
  if (queue_id == NULL) {
    event.status = osErrorParameter;
    return event;
  }
  
  taskWoken = pdFALSE;
  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }
  
  if (inHandlerMode()) {
    if (xQueueReceiveFromISR(queue_id, &event.value.v, &taskWoken) == pdTRUE) {
      /* We have mail */
      event.status = osEventMessage;
    }
    else {
      event.status = osOK;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xQueueReceive(queue_id, &event.value.v, ticks) == pdTRUE) {
      /* We have mail */
      event.status = osEventMessage;
    }
    else {
      event.status = (ticks == 0) ? osOK : osEventTimeout;
    }
  }
  
  return event;
}


/**
* @brief  Receive an item from a queue without removing the item from the queue.
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @param  millisec  timeout value or 0 in case of no time-out.
* @retval event information that includes status code.
*/
vosEvent vosMessagePeek (vosMsgQueue queue_id, uint32_t millisec)
{
  TickType_t ticks;
  vosEvent event;
  
  event.def.message_id = queue_id;
  
  if (queue_id == NULL) {
    event.status = osErrorParameter;
    return event;
  }
  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }
  
  if (xQueuePeek(queue_id, &event.value.v, ticks) == pdTRUE) 
  {
    /* We have mail */
    event.status = osEventMessage;
  }
  else 
  {
    event.status = (ticks == 0) ? osOK : osEventTimeout;
  }
  
  return event;
}

/**
* @brief  Get the number of messaged stored in a queue.
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @retval number of messages stored in a queue.
*/
uint32_t vosMessageWaiting(vosMsgQueue queue_id)
{
  if (inHandlerMode()) {
    return uxQueueMessagesWaitingFromISR(queue_id);
  }
  else
  {
    return uxQueueMessagesWaiting(queue_id);
  }
}

/**
* @brief  Get the available space in a message queue.
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @retval available space in a message queue.
*/
uint32_t vosMessageAvailableSpace(vosMsgQueue queue_id)  
{
  return uxQueueSpacesAvailable(queue_id);
}

/**
* @brief Delete a Message Queue
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @retval  status code that indicates the execution status of the function.
*/
int vosMessageDelete (vosMsgQueue queue_id)
{
  if (inHandlerMode()) {
    return osErrorISR;
  }

  vQueueDelete(queue_id);

  return osOK; 
}


typedef struct os_mailQ_cb {
  const vosMailQDef_t *queue_def;
  QueueHandle_t handle;
  vosPoolId pool;
} os_mailQ_cb_t;

vosMailQId vosMailCreate (const vosMailQDef_t *queue_def, vosThreadId thread_id)
{
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
  (void) thread_id;
  
  vosPoolDef_t pool_def = {queue_def->queue_sz, queue_def->item_sz, NULL};
  
  /* Create a mail queue control block */

  *(queue_def->cb) = pvPortMalloc(sizeof(struct os_mailQ_cb));

  if (*(queue_def->cb) == NULL) {
    return NULL;
  }
  (*(queue_def->cb))->queue_def = queue_def;
  
  /* Create a queue in FreeRTOS */
  (*(queue_def->cb))->handle = xQueueCreate(queue_def->queue_sz, sizeof(void *));


  if ((*(queue_def->cb))->handle == NULL) {
    vPortFree(*(queue_def->cb));
    return NULL;
  }
  
  /* Create a mail pool */
  (*(queue_def->cb))->pool = vosPoolCreate(&pool_def);
  if ((*(queue_def->cb))->pool == NULL) {
    //TODO: Delete queue. How to do it in FreeRTOS?
    vPortFree(*(queue_def->cb));
    return NULL;
  }
  
  return *(queue_def->cb);
#else
  return NULL;
#endif
}

void *vosMailAlloc (vosMailQId queue_id, uint32_t millisec)
{
  (void) millisec;
  void *p;
  
  
  if (queue_id == NULL) {
    return NULL;
  }
  
  p = vosPoolAlloc(queue_id->pool);
  
  return p;
}

void *vosMailCAlloc (vosMailQId queue_id, uint32_t millisec)
{
  uint32_t i;
  void *p = vosMailAlloc(queue_id, millisec);
  
  if (p) {
    for (i = 0; i < queue_id->queue_def->item_sz; i++) {
      ((uint8_t *)p)[i] = 0;
    }
  }
  
  return p;
}

int vosMailPut (vosMailQId queue_id, void *mail)
{
  portBASE_TYPE taskWoken;
  
  
  if (queue_id == NULL) {
    return osErrorParameter;
  }
  
  taskWoken = pdFALSE;
  
  if (inHandlerMode()) {
    if (xQueueSendFromISR(queue_id->handle, &mail, &taskWoken) != pdTRUE) {
      return osErrorOS;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xQueueSend(queue_id->handle, &mail, 0) != pdTRUE) { 
      return osErrorOS;
    }
  }
  
  return osOK;
}

vosEvent vosMailGet (vosMailQId queue_id, uint32_t millisec)
{
  portBASE_TYPE taskWoken;
  TickType_t ticks;
  vosEvent event;
  
  event.def.mail_id = queue_id;
  
  if (queue_id == NULL) {
    event.status = osErrorParameter;
    return event;
  }
  
  taskWoken = pdFALSE;
  
  ticks = 0;
  if (millisec == osWaitForever) {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) {
      ticks = 1;
    }
  }
  
  if (inHandlerMode()) {
    if (xQueueReceiveFromISR(queue_id->handle, &event.value.p, &taskWoken) == pdTRUE) {
      /* We have mail */
      event.status = osEventMail;
    }
    else {
      event.status = osOK;
    }
    portEND_SWITCHING_ISR(taskWoken);
  }
  else {
    if (xQueueReceive(queue_id->handle, &event.value.p, ticks) == pdTRUE) {
      /* We have mail */
      event.status = osEventMail;
    }
    else {
      event.status = (ticks == 0) ? osOK : osEventTimeout;
    }
  }
  
  return event;
}

int vosMailFree (vosMailQId queue_id, void *mail)
{
  if (queue_id == NULL) {
    return osErrorParameter;
  }
  
  return vosPoolFree(queue_id->pool, mail);
}



/*************************** Additional specific APIs to Free RTOS ************/
/**
* @brief  Handles the tick increment
* @param  none.
* @retval none.
*/
void vosSystickHandler(void)
{

#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
    xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  }
#endif  /* INCLUDE_xTaskGetSchedulerState */  
}

int vosThreadSuspend (vosThreadId thread_id)
{
#if (INCLUDE_vTaskSuspend == 1)
    vTaskSuspend(thread_id);
  
  return osOK;
#else
  return osErrorResource;
#endif
}

int vosThreadResume (vosThreadId thread_id)
{
#if (INCLUDE_vTaskSuspend == 1)  
  if(inHandlerMode())
  {
    if (xTaskResumeFromISR(thread_id) == pdTRUE)
    {
      portYIELD_FROM_ISR(pdTRUE);
    }
  }
  else
  {
    vTaskResume(thread_id);
  }
  return osOK;
#else
  return osErrorResource;
#endif
}

int vosThreadSuspendAll (void)
{
  vTaskSuspendAll();
  
  return osOK;
}

int vosThreadResumeAll (void)
{
  if (xTaskResumeAll() == pdTRUE)
    return osOK;
  else
    return osErrorOS;
  
}

/**
* @brief  Delay a task until a specified time
* @param   PreviousWakeTime   Pointer to a variable that holds the time at which the 
*          task was last unblocked. PreviousWakeTime must be initialised with the current time
*          prior to its first use (PreviousWakeTime = osKernelSysTick() )
* @param   millisec    time delay value
* @retval  status code that indicates the execution status of the function.
*/
int vosDelayUntil (uint32_t *PreviousWakeTime, uint32_t millisec)
{
#if INCLUDE_vTaskDelayUntil
  TickType_t ticks = (millisec / portTICK_PERIOD_MS);
  vTaskDelayUntil((TickType_t *) PreviousWakeTime, ticks ? ticks : 1);
  
  return osOK;
#else
  (void) millisec;
  (void) PreviousWakeTime;
  
  return osErrorResource;
#endif
}

/**
* @brief   Abort the delay for a specific thread
* @param   thread_id   thread ID obtained by \ref vosThreadCreate or \ref osThreadGetId   
* @retval  status code that indicates the execution status of the function.
*/
int vosAbortDelay(vosThreadId thread_id)
{
#if INCLUDE_xTaskAbortDelay
  
  xTaskAbortDelay(thread_id);
  
  return osOK;
#else
  (void) thread_id;
  
  return osErrorResource;
#endif
}

/**
* @brief   Lists all the current threads, along with their current state 
*          and stack usage high water mark.
* @param   buffer   A buffer into which the above mentioned details
*          will be written
* @retval  status code that indicates the execution status of the function.
*/
int vosThreadList (uint8_t *buffer)
{
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
  vTaskList((char *)buffer);
#endif
  return osOK;
}


/**
* @brief  Create and Initialize a Recursive Mutex
* @param  mutex_def     mutex definition referenced with \ref osMutex.
* @retval  mutex ID for reference by other functions or NULL in case of error..
*/
vosMutex vosRecursiveMutexCreate (const vosMutexDef_t *mutex_def)
{
#if (configUSE_RECURSIVE_MUTEXES == 1)
#if( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

  if (mutex_def->controlblock != NULL){
    return xSemaphoreCreateRecursiveMutexStatic( mutex_def->controlblock );
  }
  else {
    return xSemaphoreCreateRecursiveMutex();
  }
#elif ( configSUPPORT_STATIC_ALLOCATION == 1 )
  return xSemaphoreCreateRecursiveMutexStatic( mutex_def->controlblock );
#else 
  return xSemaphoreCreateRecursiveMutex();
#endif
#else
  return NULL;
#endif	
}

/**
* @brief  Release a Recursive Mutex
* @param   mutex_id      mutex ID obtained by \ref osRecursiveMutexCreate.
* @retval  status code that indicates the execution status of the function.
*/
int vosRecursiveMutexRelease (vosMutex mutex_id)
{
#if (configUSE_RECURSIVE_MUTEXES == 1)
  int result = osOK;
 
  if (xSemaphoreGiveRecursive(mutex_id) != pdTRUE) 
  {
    result = osErrorOS;
  }
  return result;
#else
	return osErrorResource;
#endif
}

/**
* @brief  Release a Recursive Mutex
* @param   mutex_id    mutex ID obtained by \ref osRecursiveMutexCreate.
* @param millisec      timeout value or 0 in case of no time-out.
* @retval  status code that indicates the execution status of the function.
*/
int vosRecursiveMutexWait (vosMutex mutex_id, uint32_t millisec)
{
#if (configUSE_RECURSIVE_MUTEXES == 1)
  TickType_t ticks;
  
  if (mutex_id == NULL)
  {
    return osErrorParameter;
  }
  
  ticks = 0;
  if (millisec == osWaitForever) 
  {
    ticks = portMAX_DELAY;
  }
  else if (millisec != 0) 
  {
    ticks = millisec / portTICK_PERIOD_MS;
    if (ticks == 0) 
    {
      ticks = 1;
    }
  }
  
  if (xSemaphoreTakeRecursive(mutex_id, ticks) != pdTRUE) 
  {
    return osErrorOS;
  }
  return osOK;
#else
	return osErrorResource;
#endif
}

/**
* @brief  Returns the current count value of a counting semaphore
* @param  semaphore_id  semaphore_id ID obtained by \ref osSemaphoreCreate.
* @retval  count value
*/
uint32_t vosSemaphoreGetCount(vosSem semaphore_id)
{
  return uxSemaphoreGetCount(semaphore_id);
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


