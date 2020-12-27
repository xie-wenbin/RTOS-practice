#ifndef _VOS_H_
#define _VOS_H_

#include "DtypeStm32.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef void (*vos_pthread) (void const *argument);


typedef struct vos_task_def {
    char                   *name;        ///< Thread name 
    vos_pthread             pthread;      ///< start address of thread function
    UBaseType_t             tpriority;    ///< initial thread priority
    uint32_t                stacksize;    ///< stack size requirements in bytes; 0 is default stack size
    TaskHandle_t            handle;
    
}vosThreadDef_t;




TaskHandle_t VOS_ThreadCreate (const vosThreadDef_t *thread_def, void *argument);

int VOS_ThreadTerminate (TaskHandle_t thread_id);

#endif// _VOS_H_

