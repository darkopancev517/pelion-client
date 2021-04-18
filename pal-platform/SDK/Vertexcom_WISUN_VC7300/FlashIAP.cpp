#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "FlashIAP.h"

#include "flash_api.h"

namespace mbed {

static inline bool is_aligned(uint32_t number, uint32_t alignment)
{
    if ((number % alignment) != 0) {
        return false;
    } else {
        return true;
    }
}

int FlashIAP::init()
{
    int ret = 0;

    if (flash_init(NULL) != 0) {
        ret = -1;
    }

    return ret;
}

int FlashIAP::deinit()
{
    int ret = 0;

    if (flash_free(NULL) != 0) {
        ret = -1;
    }

    return ret;
}

int FlashIAP::read(void *buffer, uint32_t address, uint32_t size)
{
    return flash_read(NULL, address, (uint8_t *) buffer, size);
}

int FlashIAP::program(const void *buffer, uint32_t address, uint32_t size)
{
    return flash_program_page(NULL, address, (const uint8_t*) buffer, size);
}

bool FlashIAP::is_aligned_to_sector(uint32_t address, uint32_t size)
{
    uint32_t current_sector_size = flash_get_sector_size(NULL, address);
    if (!is_aligned(size, current_sector_size) ||
            !is_aligned(address, current_sector_size)) {
        return false;
    } else {
        return true;
    }
}

int FlashIAP::erase(uint32_t address, uint32_t size)
{
    uint32_t flash_size = flash_get_size(NULL);
    uint32_t flash_start_addr = flash_get_start_address(NULL);
    uint32_t flash_end_addr = flash_start_addr + flash_size;
    uint32_t erase_end_addr = address + size;

    if (erase_end_addr > flash_end_addr) {
        return -1;
    } else if (erase_end_addr < flash_end_addr) {
        uint32_t following_sector_size = flash_get_sector_size(NULL, erase_end_addr);
        if (!is_aligned(erase_end_addr, following_sector_size)) {
            return -1;
        }
    }

    int32_t ret = 0;

    while ((address < erase_end_addr) && (ret == 0)) {

        ret = flash_erase_sector(NULL, address);
        uint32_t current_sector_size = flash_get_sector_size(NULL, address);

        address += current_sector_size;
    }

    return ret;
}

uint32_t FlashIAP::get_page_size() const
{
    return flash_get_page_size(NULL);
}

uint32_t FlashIAP::get_sector_size(uint32_t address) const
{
    return flash_get_sector_size(NULL, address);
}

uint32_t FlashIAP::get_flash_start() const
{
    return flash_get_start_address(NULL);
}

uint32_t FlashIAP::get_flash_size() const
{
    return flash_get_size(NULL);
}

uint8_t FlashIAP::get_erase_value() const
{
    return flash_get_erase_value(NULL);
}

}
