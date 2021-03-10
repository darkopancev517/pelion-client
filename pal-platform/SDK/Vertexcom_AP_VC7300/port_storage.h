#ifndef PORT_STORAGE_H
#define PORT_STORAGE_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t port_storage_init(void);

int32_t port_storage_deinit(void);

int32_t port_storage_read(uint32_t address, uint8_t* data, uint32_t size);

int32_t port_storage_program_page(uint32_t address, const uint8_t* data, uint32_t size);

int32_t port_storage_erase_sector(uint32_t address);

uint32_t port_storage_get_sector_size(uint32_t address);

uint32_t port_storage_get_page_size(uint32_t address);

uint32_t port_storage_get_start_address(void);

uint32_t port_storage_get_size(void);

uint8_t port_storage_get_erase_value(void);

#ifdef __cplusplus
}
#endif

#endif // PORT_STORAGE_H
