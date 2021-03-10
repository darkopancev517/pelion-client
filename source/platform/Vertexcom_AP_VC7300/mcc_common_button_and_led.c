#include "mcc_common_config.h"
#include "mcc_common_button_and_led.h"
#include <stdint.h>

uint8_t mcc_platform_button_clicked(void)
{
    return 0;
}

uint8_t mcc_platform_init_button_and_led(void)
{
   return 0;
}

void mcc_platform_toggle_led(void)
{
}

void mcc_platform_led_off(void)
{
}
