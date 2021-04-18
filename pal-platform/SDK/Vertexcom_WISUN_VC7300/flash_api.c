#include "flash_api.h"

#include "port_storage.h"

int32_t flash_init(flash_t *obj)
{
    (void) obj;

    return port_storage_init();
}

int32_t flash_free(flash_t *obj)
{
    (void) obj;

    return port_storage_deinit();
}

int32_t flash_erase_sector(flash_t *obj, uint32_t address)
{
    (void) obj;

    return port_storage_erase_sector(address);
}

int32_t flash_read(flash_t *obj, uint32_t address, uint8_t *data, uint32_t size)
{
    (void) obj;

    return port_storage_read(address, data, size);
}

int32_t flash_program_page(flash_t *obj, uint32_t address, const uint8_t *data, uint32_t size)
{
    (void) obj;

    return port_storage_program_page(address, data, size);
}

uint32_t flash_get_sector_size(const flash_t *obj, uint32_t address)
{
    (void) obj;

    uint32_t size = port_storage_get_sector_size(address);

    return (size) ? size : MBED_FLASH_INVALID_SIZE;
}

uint32_t flash_get_page_size(const flash_t *obj)
{
    (void) obj;

    return port_storage_get_page_size(port_storage_get_start_address());
}

uint32_t flash_get_start_address(const flash_t *obj)
{
    (void) obj;

    return port_storage_get_start_address();
}

uint32_t flash_get_size(const flash_t *obj)
{
    (void) obj;

    return port_storage_get_size();
}

uint8_t flash_get_erase_value(const flash_t *obj)
{
    (void) obj;

    return port_storage_get_erase_value();
}
