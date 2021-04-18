#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

#define TRACE_GROUP "PAL"

#define PAL_RTOS_TRANSLATE_CMSIS_ERROR_CODE(cmsisCode)\
    ((int32_t)(cmsisCode + PAL_ERR_RTOS_ERROR_BASE))

#define PAL_TICK_TO_MILLI_FACTOR 1000

palStatus_t pal_plat_RTOSInitialize(void *opaqueContext)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_RTOSDestroy(void)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osDelay(uint32_t milliseconds)
{
    return PAL_SUCCESS;
}

uint64_t pal_plat_osKernelSysTick()
{
    return 0;
}

uint64_t pal_plat_osKernelSysTickMicroSec(uint64_t microseconds)
{
    return 0;
}

uint64_t pal_plat_osKernelSysTickFrequency()
{
    return 0;
}

palStatus_t pal_plat_osThreadCreate(palThreadFuncPtr function, void* funcArgument, palThreadPriority_t priority, uint32_t stackSize, palThreadID_t* threadID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palThreadID_t pal_plat_osThreadGetId(void)
{
    palThreadID_t threadID = 0;
    return threadID;
}

palStatus_t pal_plat_osThreadTerminate(palThreadID_t* threadID)
{
    palStatus_t status = PAL_ERR_RTOS_TASK;
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
    return status;
}

palStatus_t pal_plat_osMutexDelete(palMutexID_t* mutexID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osSemaphoreCreate(uint32_t count, palSemaphoreID_t* semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osSemaphoreWait(palSemaphoreID_t semaphoreID, uint32_t millisec, int32_t* countersAvailable)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osSemaphoreRelease(palSemaphoreID_t semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osSemaphoreDelete(palSemaphoreID_t* semaphoreID)
{
    palStatus_t status = PAL_SUCCESS;
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
