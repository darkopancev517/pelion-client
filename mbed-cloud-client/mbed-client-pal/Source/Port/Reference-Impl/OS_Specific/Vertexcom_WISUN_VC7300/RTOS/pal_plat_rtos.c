#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

#include <vcrtos/thread.h>
#include <vcrtos/mutex.h>
#include <vcrtos/cpu.h>
#include <vcrtos/sema.h>

#define TRACE_GROUP "PAL"

#define PAL_RTOS_TRANSLATE_CMSIS_ERROR_CODE(cmsisCode)\
    ((int32_t)(cmsisCode + PAL_ERR_RTOS_ERROR_BASE))

#define PAL_TICK_TO_MILLI_FACTOR 1000

typedef struct palMutex
{
    palMutexID_t mutexID;
} palMutex_t;

typedef struct palSemaphore
{
    palSemaphoreID_t semaphoreID;
    uint32_t maxCount;
} palSemaphore_t;

typedef struct palThreadData
{
    char *allocatedStackPtr;
    kernel_pid_t threadPID;
    thread_t *sysThreadID;
    palThreadFuncPtr userFunction;
    void *userFunctionArgument;
} palThreadData_t;

typedef struct palTimer
{
    palTimerID_t timerID;
    void *functionArgs;
} palTimer_t;

#define PAL_MAX_CONCURRENT_THREADS 8

PAL_PRIVATE palMutexID_t g_threadsMutex = NULLPTR;
PAL_PRIVATE palThreadData_t *g_threadsArray[PAL_MAX_CONCURRENT_THREADS] = { 0 };

#define PAL_THREADS_MUTEX_LOCK(status) \
    { \
        status = pal_osMutexWait(g_threadsMutex, PAL_RTOS_WAIT_FOREVER); \
        if (PAL_SUCCESS != status) \
        { \
            PAL_LOG_ERR("%s mutex wait failed\n", __FUNCTION__); \
        } \
    }

#define PAL_THREADS_MUTEX_UNLOCK(status) \
    { \
        status = pal_osMutexRelease(g_threadsMutex); \
        if (PAL_SUCCESS != status) \
        { \
            PAL_LOG_ERR("%s mutex release failed\n", __FUNCTION__); \
        } \
    }

PAL_PRIVATE void threadFree(palThreadData_t** threadData);
PAL_PRIVATE palThreadData_t** threadFind(const thread_t *thread);
PAL_PRIVATE PAL_INLINE palThreadData_t** threadAllocate(void);
PAL_PRIVATE void *osMutexCreate(void);
PAL_PRIVATE void osMutexDestroy(uintptr_t mutexID);
PAL_PRIVATE uintptr_t osSemaphoreCreate(unsigned int value);
PAL_PRIVATE PAL_INLINE uint32_t pal_plat_GetIPSR(void);
PAL_PRIVATE void *threadFunction(void *arg);

palStatus_t pal_plat_RTOSInitialize(void *opaqueContext)
{
    palStatus_t status = pal_osMutexCreate(&g_threadsMutex);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
    PAL_THREADS_MUTEX_LOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
    for (int i = 0; i < PAL_MAX_CONCURRENT_THREADS; i++)
    {
        if (g_threadsArray[i])
        {
            threadFree(&g_threadsArray[i]);
        }
    }
    PAL_THREADS_MUTEX_UNLOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
end:
    return status;
}

palStatus_t pal_plat_RTOSDestroy(void)
{
    palStatus_t status = PAL_SUCCESS;
    if (g_threadsMutex != NULLPTR)
    {
        status = pal_osMutexDelete(&g_threadsMutex);
        g_threadsMutex = NULLPTR;
    }
    return status;
}

palStatus_t pal_plat_osDelay(uint32_t milliseconds)
{
    // TODO
    return PAL_SUCCESS;
}

uint64_t pal_plat_osKernelSysTick()
{
    // TODO
    return 0;
}

uint64_t pal_plat_osKernelSysTickMicroSec(uint64_t microseconds)
{
    // TODO
    return 0;
}

uint64_t pal_plat_osKernelSysTickFrequency()
{
    // TODO
    return 0;
}

palStatus_t pal_plat_osThreadCreate(palThreadFuncPtr function, void* funcArgument, palThreadPriority_t priority, uint32_t stackSize, palThreadID_t* threadID)
{
    palStatus_t status = PAL_SUCCESS;
    palThreadData_t** threadData;

    PAL_THREADS_MUTEX_LOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
    threadData = threadAllocate(); // allocate thread data from the global array
    PAL_THREADS_MUTEX_UNLOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }

    if (threadData == NULL) // allocation failed or all array slots are occupied
    {
        status = PAL_ERR_RTOS_RESOURCE;
        goto end;
    }

    (*threadData)->userFunction = function;
    (*threadData)->userFunctionArgument = funcArgument;
    (*threadData)->allocatedStackPtr = (char *)malloc(stackSize);

    if ((*threadData)->allocatedStackPtr == NULL)
    {
        status = PAL_ERR_NO_MEMORY;
        goto end;
    }

    (*threadData)->threadPID = thread_create((*threadData)->allocatedStackPtr, stackSize, threadFunction,
                                             "palTask", (uint8_t)priority, (void *)threadData,
                                             THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKMARKER);

    PAL_THREADS_MUTEX_LOCK(status);
    if ((*threadData)->threadPID != KERNEL_PID_UNDEF)
    {
        (*threadData)->sysThreadID = thread_get_from_scheduler((*threadData)->threadPID);
        *threadID = (palThreadID_t)(*threadData)->sysThreadID;
    }
    else
    {
        status = PAL_ERR_GENERIC_FAILURE;
        free((*threadData)->allocatedStackPtr);
        threadFree(threadData);
        PAL_LOG_ERR("Rtos thread create failure");
    }
    PAL_THREADS_MUTEX_UNLOCK(status);

end:
    return status;
}

palThreadID_t pal_plat_osThreadGetId(void)
{
    palThreadID_t threadID = (palThreadID_t)thread_current();
    return threadID;
}

palStatus_t pal_plat_osThreadTerminate(palThreadID_t* threadID)
{
    palStatus_t status = PAL_ERR_RTOS_TASK;
    thread_t *sysThreadID = (thread_t *)threadID;
    palThreadData_t** threadData;
    if (thread_current() != sysThreadID) // self termination not allowed
    {
        PAL_THREADS_MUTEX_LOCK(status);
        if (status != PAL_SUCCESS)
        {
            goto end;
        }
        threadData = threadFind(sysThreadID);
        if (threadData)
        {
            thread_terminate(sysThreadID->pid);
            free((*threadData)->allocatedStackPtr);
            threadFree(threadData);
        }
        PAL_THREADS_MUTEX_UNLOCK(status);
    }
end:
    return status;
}

palStatus_t pal_plat_osTimerCreate(palTimerFuncPtr function, void* funcArgument, palTimerType_t timerType, palTimerID_t* timerID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osTimerStart(palTimerID_t timerID, uint32_t millisec)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osTimerStop(palTimerID_t timerID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osTimerDelete(palTimerID_t* timerID)
{
    palStatus_t status = PAL_ERR_RTOS_PARAMETER;
    return status;
}

palStatus_t pal_plat_osMutexCreate(palMutexID_t* mutexID)
{
    palStatus_t status = PAL_SUCCESS;
    palMutex_t *mutex = NULL;
    if (mutexID == NULL)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }
    mutex = (palMutex_t*)malloc(sizeof(palMutex_t));
    if (NULL == mutex)
    {
        status = PAL_ERR_NO_MEMORY;
    }
    if (PAL_SUCCESS == status)
    {
        mutex->mutexID = (uintptr_t)osMutexCreate();
        if (mutex->mutexID == NULLPTR)
        {
            free(mutex);
            mutex = NULL;
            PAL_LOG_ERR("Rtos mutex create failure");
            status = PAL_ERR_GENERIC_FAILURE;
        }
        *mutexID = (palMutexID_t)mutex;
    }
    return status;
}

palStatus_t pal_plat_osMutexWait(palMutexID_t mutexID, uint32_t millisec)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osMutexRelease(palMutexID_t mutexID)
{
    palStatus_t status = PAL_SUCCESS;
    palMutex_t *mutex = NULL;

    if (mutexID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }
    mutex = (palMutex_t *)mutexID;
    mutex_unlock((mutex_t *)mutex->mutexID);
    return status;
}

palStatus_t pal_plat_osMutexDelete(palMutexID_t* mutexID)
{
    palStatus_t status = PAL_SUCCESS;
    palMutex_t *mutex = NULL;

    if (mutexID == NULL || *mutexID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    mutex = (palMutex_t *)*mutexID;
    if (mutex->mutexID != NULLPTR)
    {
        osMutexDestroy(mutex->mutexID);
        free(mutex);
        *mutexID = NULLPTR;
        status = PAL_SUCCESS;
    }
    else
    {
        PAL_LOG_ERR("Rtos mutex delete failure");
        status = PAL_ERR_GENERIC_FAILURE;
    }

    return status;
}

palStatus_t pal_plat_osSemaphoreCreate(uint32_t count, palSemaphoreID_t* semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
    palSemaphore_t *semaphore = NULL;

    if (NULL == semaphoreID)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    semaphore = (palSemaphore_t *)malloc(sizeof(palSemaphore_t));
    if (semaphore == NULL)
    {
        status = PAL_ERR_NO_MEMORY;
    }

    if (status == PAL_SUCCESS)
    {
        semaphore->semaphoreID = osSemaphoreCreate(count);
        semaphore->maxCount = PAL_SEMAPHORE_MAX_COUNT;
        if (semaphore->semaphoreID == NULLPTR)
        {
            free(semaphore);
            semaphore = NULLPTR;
            PAL_LOG_ERR("Rtos semaphore create error");
            status = PAL_ERR_GENERIC_FAILURE;
        }
        else
        {
            *semaphoreID = (palSemaphoreID_t)semaphore;
        }
    }
    return status;
}

palStatus_t pal_plat_osSemaphoreWait(palSemaphoreID_t semaphoreID, uint32_t millisec, int32_t* countersAvailable)
{
    palStatus_t status = PAL_SUCCESS;
    // TODO
    return status;
}

palStatus_t pal_plat_osSemaphoreRelease(palSemaphoreID_t semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
    palSemaphore_t *semaphore = NULL;
    int32_t tmpCounters = 0;
    int res = 0;

    if (semaphoreID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    semaphore = (palSemaphore_t*)semaphoreID;

    tmpCounters = ((sema_t *)(semaphore->semaphoreID))->value;

    if (tmpCounters < semaphore->maxCount)
    {
        res = sema_post((sema_t *)semaphore->semaphoreID);
        if (res != 0)
        {
            status = PAL_ERR_RTOS_PARAMETER;
        }
    }
    else
    {
        status = PAL_ERR_RTOS_RESOURCE;
    }

    return status;
}

palStatus_t pal_plat_osSemaphoreDelete(palSemaphoreID_t* semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
    palSemaphore_t *semaphore = NULL;

    if (semaphoreID == NULL || *semaphoreID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    semaphore = (palSemaphore_t *)*semaphoreID;
    if (semaphore->semaphoreID != NULLPTR)
    {
        sema_destroy((sema_t *)semaphore->semaphoreID);
        free(semaphore);
        *semaphoreID = NULLPTR;
        status = PAL_SUCCESS;
    }
    else
    {
        PAL_LOG_ERR("Rtos semaphore destroy error");
        status = PAL_ERR_GENERIC_FAILURE;
    }

    return status;
}

void *pal_plat_malloc(size_t len)
{
    return NULL;
}

void pal_plat_free(void *buffer)
{
    (void)buffer;
}

palStatus_t pal_plat_osRandomBuffer(uint8_t *randomBuf, size_t bufSizeBytes, size_t* actualRandomSizeBytes)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

// PRIVATE FUNCTIONS -----------------------------------------------------------

PAL_PRIVATE void threadFree(palThreadData_t** threadData)
{
    (*threadData)->userFunction = NULL;
    (*threadData)->userFunctionArgument = NULL;
    (*threadData)->sysThreadID = NULL;
    (*threadData)->threadPID = KERNEL_PID_UNDEF;
    free(*threadData);
    *threadData = NULL;
}

PAL_PRIVATE palThreadData_t** threadFind(const thread_t *thread)
{
    palThreadData_t** threadData = NULL;
    for (int i = 0; i < PAL_MAX_CONCURRENT_THREADS; i++)
    {
        if (thread == g_threadsArray[i]->sysThreadID)
        {
            threadData = &g_threadsArray[i];
            break;
        }
    }
    return threadData;
}

PAL_PRIVATE PAL_INLINE palThreadData_t** threadAllocate(void)
{
    palThreadData_t** threadData = NULL;
    for (int i = 0; i < PAL_MAX_CONCURRENT_THREADS; i++)
    {
        if (!g_threadsArray[i])
        {
            g_threadsArray[i] = (palThreadData_t*)malloc(sizeof(palThreadData_t));
            if (g_threadsArray[i])
            {
                threadData = &g_threadsArray[i];
            }
            break;
        }
    }
    return threadData;
}

PAL_PRIVATE void *osMutexCreate(void)
{
    mutex_t *mutex = (mutex_t*)malloc(sizeof(mutex_t));
    if (mutex == NULL)
    {
        return NULLPTR;
    }
    mutex_init(mutex);
    return (void *)mutex;
}

PAL_PRIVATE void osMutexDestroy(uintptr_t mutexID)
{
    mutex_t *mutex = (mutex_t *)mutexID;
    if (mutex == NULL)
    {
        return;
    }
    mutex_unlock(mutex);
    free(mutex);
}

PAL_PRIVATE uintptr_t osSemaphoreCreate(unsigned int value)
{
    sema_t *sema = (sema_t *)malloc(sizeof(sema_t));
    if (sema == NULL)
    {
        return NULLPTR;
    }
    sema_create(sema, value);
    return (uintptr_t)sema;
}

PAL_PRIVATE PAL_INLINE uint32_t pal_plat_GetIPSR(void)
{
	uint32_t result;
	__asm volatile ("MRS %0, ipsr" : "=r" (result) );
	return(result);
}

PAL_PRIVATE void *threadFunction(void *arg)
{
    palStatus_t status = PAL_SUCCESS;
    palThreadData_t** threadData;
    palThreadFuncPtr userFunction;
    void *userFunctionArgument;
    char *allocatedStackPtr = NULL;

    PAL_THREADS_MUTEX_LOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
    threadData = (palThreadData_t**)arg;
    userFunction = (*threadData)->userFunction;
    userFunctionArgument = (*threadData)->userFunctionArgument;
    allocatedStackPtr = (*threadData)->allocatedStackPtr;
    if ((*threadData)->sysThreadID == NULL)
    {
        (*threadData)->sysThreadID = thread_current();
    }
    PAL_THREADS_MUTEX_UNLOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }

    userFunction(userFunctionArgument); // invoke user function

    PAL_THREADS_MUTEX_LOCK(status);
    if (status != PAL_SUCCESS)
    {
        goto end;
    }
    threadFree(threadData);
    PAL_THREADS_MUTEX_UNLOCK(status);

end:
    thread_exit();
    if (allocatedStackPtr != NULL)
    {
        free(allocatedStackPtr);
    }
    cpu_switch_context_exit();
    return NULL;
}
