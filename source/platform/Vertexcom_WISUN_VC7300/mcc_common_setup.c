#include "mcc_common_setup.h"
#include "pal.h"

int mcc_platform_init_connection(void)
{
    return 0;
}

void* mcc_platform_get_network_interface(void)
{
    return NULL;
}

int mcc_platform_close_connection(void)
{
    return 0;
}

int mcc_platform_interface_connect(void)
{
    return 0;
}

int mcc_platform_interface_close(void)
{
    // XXX: Ignore the fact that this is not implemented.
    return 0;
}

void* mcc_platform_interface_get(void)
{
    return 0;
}

void mcc_platform_interface_init(void)
{
}

int mcc_platform_reformat_storage(void)
{
    return 0;
}

int mcc_platform_storage_init(void)
{
    return 0;
}

int mcc_platform_init(void)
{
    return 0;
}

void mcc_platform_do_wait(int timeout_ms)
{
    (void)timeout_ms;
}

void mcc_platform_sw_build_info(void)
{
}

void mcc_platform_reboot(void)
{
}
