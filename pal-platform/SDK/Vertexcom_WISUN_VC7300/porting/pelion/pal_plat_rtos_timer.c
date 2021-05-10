#include "contiki.h"

#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

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
