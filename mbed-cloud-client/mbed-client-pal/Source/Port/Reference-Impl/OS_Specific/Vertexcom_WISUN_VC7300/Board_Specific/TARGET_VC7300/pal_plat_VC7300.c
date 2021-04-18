#include "pal_plat_rtos.h"

int32_t pal_plat_osAtomicIncrement(int32_t* valuePtr, int32_t increment)
{
    return 0;
}

void pal_plat_osReboot()
{
}

palStatus_t pal_plat_getRandomBufferFromHW(uint8_t *randomBuf, size_t bufSizeBytes, size_t* actualRandomSizeBytes)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

#if (PAL_USE_HW_RTC)
palMutexID_t rtcMutex = NULLPTR;
palStatus_t pal_plat_osGetRtcTime(uint64_t *rtcGetTime)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_osSetRtcTime(uint64_t rtcSetTime)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_rtcInit(void)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_rtcDeInit(void)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}
#endif
