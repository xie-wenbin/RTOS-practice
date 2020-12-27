#include "vos.h"




TaskHandle_t VOS_ThreadCreate (const vosThreadDef_t *thread_def, void *argument)
{
    BaseType_t xReturn = pdPASS;
    TaskHandle_t  handle;
    
    taskENTER_CRITICAL();

    xReturn = xTaskCreate((TaskFunction_t )thread_def->pthread,
                          (const char*    )thread_def->name,
                          thread_def->stacksize,
                          argument,
                          thread_def->tpriority,
                          &handle);
    
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

