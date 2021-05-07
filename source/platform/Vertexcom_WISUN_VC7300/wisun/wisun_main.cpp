#include <stdint.h>

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include "pal.h"
#include "pal_plat_rtos.h"
#include <stdlib.h>

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

#if 0
static void main_arm_pelion_application(void);


// Pointer to mbedClient, used for calling close function.
static SimpleM2MClient *client;

void main_arm_pelion_application(void)
{
    // Initialize storage
    if (mcc_platform_storage_init() != 0) {
        printf("Failed to initialize storage\r\n");
        return;
    }

    // Initialize platform-specific components
    if (mcc_platform_init() != 0) {
        printf("ERROR - platform_init() failed!\r\n");
        return;
    }

    // Print some statistics of the object sizes and their heap memory consumption.
    // NOTE: This *must* be done before creating MbedCloudClient, as the statistic calculation
    // creates and deletes M2MSecurity and M2MDevice singleton objects, which are also used by
    // the MbedCloudClient.
#ifdef MEMORY_TESTS_HEAP
    print_m2mobject_stats();
#endif

    // SimpleClient is used for registering and unregistering resources to a server.
    SimpleM2MClient mbedClient;

    // Save pointer to mbedClient so that other functions can access it.
    client = &mbedClient;

    /*
     * Pre-initialize network stack and client library.
     *
     * Specifically for nanostack mesh networks on Mbed OS platform it is important to initialize
     * the components in correct order to avoid out-of-memory issues in Device Management Client initialization.
     * The order for these use cases should be:
     * 1. Initialize network stack using `nsapi_create_stack()` (Mbed OS only). // Implemented in `mcc_platform_interface_init()`.
     * 2. Initialize Device Management Client using `init()`.                   // Implemented in `mbedClient.init()`.
     * 3. Connect to network interface using 'connect()`.                       // Implemented in `mcc_platform_interface_connect()`.
     * 4. Connect Device Management Client to service using `setup()`.          // Implemented in `mbedClient.register_and_connect)`.
     */
    (void) mcc_platform_interface_init();
    mbedClient.init();

#if 0
    // application_init() runs the following initializations:
    //  1. platform initialization
    //  2. print memory statistics if MEMORY_TESTS_HEAP is defined
    //  3. FCC initialization.
    if (!application_init()) {
        printf("Initialization failed, exiting application!\r\n");
        return;
    }
#endif

#if defined MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER &&\
 (MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER == 1)
    printf("Configuring Interface\r\n");
    if (network_manager.reg_and_config_iface(NetworkInterface::get_default_instance()) != NM_ERROR_NONE) {
        printf("Failed to register and configure Interface\r\n");
        return;
    }
#endif

    // Print platform information
    mcc_platform_sw_build_info();

    // Initialize network
    int timeout_ms = 1000;
    while (-1 == mcc_platform_interface_connect()){
        // Will try to connect using mcc_platform_interface_connect forever. 
        // wait timeout is always doubled
        printf("Network connect failed. Try again after %d milliseconds.\n",timeout_ms);
        mcc_platform_do_wait(timeout_ms);
        timeout_ms *= 2;
    }
    printf("Network initialized, registering...\r\n");    

#ifdef MEMORY_TESTS_HEAP
    printf("Client initialized\r\n");
    print_heap_stats();
#endif

#if 0
#if defined MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER &&\
 (MBED_CONF_MBED_CLOUD_CLIENT_NETWORK_MANAGER == 1)
    network_manager.create_resource(mbedClient.get_m2m_obj_list());
#endif

    mbedClient.register_and_connect();
#endif
}
#endif

palMutexID_t mutex1 = NULLPTR;
//palMutexID_t mutex2 = NULLPTR;
//palSemaphoreID_t semaphore1 = NULLPTR;

int main(void)
{
    wisun_main_system_init();

    palStatus_t status = PAL_SUCCESS;

    status = pal_osMutexCreate(&mutex1);

    if (status != PAL_SUCCESS || mutex1 == NULLPTR)
    {
        printf("mutex create failed\n");
    }

    status = pal_osMutexDelete(&mutex1);

    if (status != PAL_SUCCESS || mutex1 != NULLPTR)
    {
        printf("mutex delete failed\n");
    }

    //main_arm_pelion_application();

    while (1)
    {
        thread_sleep();
    }

    // should not reach here.

    return 0;
}
