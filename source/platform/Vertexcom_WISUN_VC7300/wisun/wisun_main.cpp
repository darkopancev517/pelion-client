#include <stdint.h>

// Needed for PRIu64 on FreeRTOS
#include <stdio.h>
// Note: this macro is needed on armcc to get the the limit macros like UINT16_MAX
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

// Note: this macro is needed on armcc to get the the PRI*32 macros
// from inttypes.h in a C++ code.
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#if defined (__RTX)
#include "pdmc_main.h"
#endif
#include "simplem2mclient.h"
#ifdef TARGET_LIKE_MBED
#include "mbed.h"
#endif
#include "application_init.h"
#include "mcc_common_button_and_led.h"
#include "blinky.h"
#ifndef MBED_CONF_MBED_CLOUD_CLIENT_DISABLE_CERTIFICATE_ENROLLMENT
#include "certificate_enrollment_user_cb.h"
#endif

#if defined(MBED_CONF_NANOSTACK_HAL_EVENT_LOOP_USE_MBED_EVENTS) && \
 (MBED_CONF_NANOSTACK_HAL_EVENT_LOOP_USE_MBED_EVENTS == 1) && \
 defined(MBED_CONF_EVENTS_SHARED_DISPATCH_FROM_APPLICATION) && \
 (MBED_CONF_EVENTS_SHARED_DISPATCH_FROM_APPLICATION == 1)
#include "nanostack-event-loop/eventOS_scheduler.h"
#endif

#ifdef MBED_CLOUD_CLIENT_SUPPORT_MULTICAST_UPDATE
#include "multicast.h"
#endif

#if defined MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER && \
 (MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER == 1)
#include "NetworkInterface.h"
#include "NetworkManager.h"
#endif

extern "C" int wisun_main_system_init(void);

int main(void)
{
    wisun_main_system_init();

    // should not reach here.

    while (1);

    return 0;
}
