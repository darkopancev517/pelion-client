#include "contiki.h"

#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

#define TRACE_GROUP "PAL"

#define PAL_TICK_TO_MILLI_FACTOR 1000
#define configTICK_RATE_HZ 1000

#ifndef PAL_MAX_NUM_OF_TIMERS
#define PAL_MAX_NUM_OF_TIMERS 16
#endif

typedef void (*TimerCallbackFunction_t)(void *arg);

typedef struct palTimer
{
    palTimerID_t timerID;
    TimerCallbackFunction_t function;
    void *functionArgs;
    uint32_t timerType;
} palTimer_t;

PAL_PRIVATE palTimer_t *s_timerArrays[PAL_MAX_NUM_OF_TIMERS] = { 0 };
PAL_PRIVATE void *osTimerCreate();
PAL_PRIVATE void osTimerDestroy(uintptr_t timerID);
PAL_PRIVATE void pal_plat_osTimerWrapperFunction(void *xtimerID);

palStatus_t pal_plat_osDelay(uint32_t milliseconds)
{
    clock_wait(milliseconds);

    // Note: blocking wait

    return PAL_SUCCESS;
}

uint64_t pal_plat_osKernelSysTick()
{
    return clock_time();
}

uint64_t pal_plat_osKernelSysTickMicroSec(uint64_t microseconds)
{
    uint64_t sysTicks = microseconds * configTICK_RATE_HZ / (PAL_TICK_TO_MILLI_FACTOR * PAL_TICK_TO_MILLI_FACTOR);
    return sysTicks;
}

uint64_t pal_plat_osKernelSysTickFrequency()
{
    return configTICK_RATE_HZ; // 1ms systick
}

palStatus_t pal_plat_osTimerCreate(palTimerFuncPtr function, void* funcArgument, palTimerType_t timerType, palTimerID_t* timerID)
{
    palStatus_t status = PAL_SUCCESS;
    palTimer_t *timer = NULL;
    int i;
    if (timerID == NULL || function == NULL)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    timer = (palTimer_t *)malloc(sizeof(palTimer_t));

    if (timer == NULL)
    {
        status = PAL_ERR_NO_MEMORY;
    }
    else
    {
        memset(timer, 0, sizeof(palTimer_t));
    }

    if (status == PAL_SUCCESS)
    {
        for (i = 0; i < PAL_MAX_NUM_OF_TIMERS; i++)
        {
            if (s_timerArrays[i] == NULL)
            {
                s_timerArrays[i] = timer;
                break;
            }
        }

        if (PAL_MAX_NUM_OF_TIMERS == i)
        {
            status = PAL_ERR_NO_MEMORY;
        }

        if (status == PAL_SUCCESS)
        {
            timer->function = (TimerCallbackFunction_t)function;
            timer->functionArgs = funcArgument;
            timer->timerType = timerType; // 0 = osTimerOnce, 1 = osTimerPeriodic
            timer->timerID = (palTimerID_t)osTimerCreate();
        }

        if (timer->timerID == NULLPTR)
        {
            free(timer);
            timer = NULLPTR;
            PAL_LOG_ERR("Rtos timer create failure");
            status = PAL_ERR_GENERIC_FAILURE;
        }
        else
        {
            *timerID = (palTimerID_t)timer;
        }
    }

    return status;
}

palStatus_t pal_plat_osTimerStart(palTimerID_t timerID, uint32_t millisec)
{
    palStatus_t status = PAL_SUCCESS;

    palTimer_t *timer = NULL;

    if (timerID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    timer = (palTimer_t *)timerID;

    if (timer->timerID != NULLPTR)
    {
        ctimer_set((struct ctimer *)timer->timerID,
                   millisec,
                   pal_plat_osTimerWrapperFunction,
                   (void *)timer->timerID);
    }
    else
    {
        PAL_LOG_ERR("Rtos timer start error");
        status = PAL_ERR_GENERIC_FAILURE;
    }

    return status;
}

palStatus_t pal_plat_osTimerStop(palTimerID_t timerID)
{
    palStatus_t status = PAL_SUCCESS;
    palTimer_t *timer = NULL;

    if (timerID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    timer = (palTimer_t *)timerID;

    if (timer->timerID != NULLPTR)
    {
        ctimer_stop((struct ctimer *)timer->timerID);
    }
    else
    {
        PAL_LOG_ERR("Rtos timer stop error");
        status = PAL_ERR_GENERIC_FAILURE;
    }

    return status;
}

palStatus_t pal_plat_osTimerDelete(palTimerID_t* timerID)
{
    palStatus_t status = PAL_SUCCESS;
    palTimer_t *timer = NULL;

    if (timerID == NULL || *timerID == NULLPTR)
    {
        return PAL_ERR_INVALID_ARGUMENT;
    }

    timer = (palTimer_t *)*timerID;

    if (timer->timerID != NULLPTR)
    {
        osTimerDestroy(timer->timerID);
        free(timer);
        *timerID = NULLPTR;
        status = PAL_SUCCESS;
    }
    else
    {
        PAL_LOG_ERR("Rtos semaphore destroy error");
        status = PAL_ERR_GENERIC_FAILURE;
    }

    return status;
}

// PRIVATE functions ----------------------------------------------------------
PAL_PRIVATE void *osTimerCreate()
{
    struct ctimer *timer = (struct ctimer *)malloc(sizeof(struct ctimer));
    if (timer == NULL)
        return NULLPTR;

    return (void *)timer;
}

PAL_PRIVATE void osTimerDestroy(uintptr_t timerID)
{
    struct ctimer *timer = (struct ctimer *)timerID;
    if (timer == NULL)
        return;

    ctimer_stop(timer);
    free(timer);
}

PAL_PRIVATE void pal_plat_osTimerWrapperFunction(void *xtimerID)
{
    palTimer_t *timer = NULL;
    for (int i = 0; i < PAL_MAX_NUM_OF_TIMERS; i++)
    {
        if (s_timerArrays[i]->timerID == (palTimerID_t)xtimerID)
        {
            timer = s_timerArrays[i];
            timer->function(timer->functionArgs);
            if (timer->timerType == 1)
            {
                ctimer_reset((struct ctimer *)xtimerID); // periodic timer
            }
        }
    }
}
