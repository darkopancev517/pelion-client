#include "port_storage.h"

int32_t port_storage_init(void)
{
    return 0;
}

int32_t port_storage_deinit(void)
{
    return 0;
}

int32_t port_storage_read(uint32_t address, uint8_t* data, uint32_t size)
{
    return 0;
}

int32_t port_storage_program_page(uint32_t address, const uint8_t* data, uint32_t size)
{
    return 0;
}

int32_t port_storage_erase_sector(uint32_t address)
{
    return 0;
}

uint32_t port_storage_get_sector_size(uint32_t address)
{
    return 0;
}

uint32_t port_storage_get_page_size(uint32_t address)
{
    return 0;
}

uint32_t port_storage_get_start_address(void)
{
    return 0;
}

uint32_t port_storage_get_size(void)
{
    return 0;
}

uint8_t port_storage_get_erase_value(void)
{
    return 0;
}
