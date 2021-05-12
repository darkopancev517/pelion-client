#include "contiki.h"

#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

void *pal_plat_malloc(size_t len)
{
    return (void *)malloc(len);
}

void pal_plat_free(void *buffer)
{
    free(buffer);
}

palStatus_t pal_plat_osRandomBuffer(uint8_t *randomBuf, size_t bufSizeBytes, size_t* actualRandomSizeBytes)
{
    palStatus_t status = PAL_SUCCESS;
    // TODO
    return status;
}
