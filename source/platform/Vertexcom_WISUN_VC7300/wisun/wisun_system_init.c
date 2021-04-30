/**
 * Copyright (c) 2018 Vertexcom Technologies, Inc. All rights reserved
 * Vertexcom Confidential Proprietary
 *
*/

/**
 * Copyright (c) 2018 Vertexcom Technologies, Inc. All rights reserved
 * Vertexcom Confidential Proprietary
 *
 * @file main.c
 *
 * @brief This file is designed for running the main process of system
 *
 */

#include "vertexcom-config.h"

#include "misc/types.h"
#include "misc/shell.h"
#include "misc/sys_hook.h"
#include "misc/shell_commands.h"

#include "hal/board.h"

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include "lib/random.h"
#include "dev/xmem.h"

#include "sys/node-id.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#if NETSTACK_CONF_WITH_WISUN
#include "net/wisun/wisun-rpl.h"
#include "net/rpl/rpl.h"
#endif

#ifdef CONFIG_CONTIKI_WITH_WISUN_SECURITY
#include "net/wisun/eapol_relay.h"
#endif

#include "gpio_config.h"

#include "system/contiki/utils/telnetd/telnetd.h"

#include "vc_pktHandler.h"
#include "vc_app_coapHandler.h"
#include "contiki.h"
#include "net/ip/uip.h"
#if CONFIG_DRV_APPUART_VC_TIMERUART
#include "vc_timerUart.h"
#endif
#if CONFIG_DRV_APPUART_VC_QGDW_1376
#include "vc_timerUart.h"
#include "vertexcom_qgdw_contiki.h"
#endif
#if CONFIG_DRV_APPUART_VERTEXCOM_SLIP
#include "vertexcom_slip.h"
#endif
#if CONFIG_DRV_APPUART_VERTEXCOM_DLMS
#include "vertexcom_dlms.h"
#endif

#if CONFIG_DRV_APPUART_ATCMD
#include "at_command/atcmd_uart.h"
#endif

#include "net/dhcp6/dhcp6.h"

#if CONFIG_WISUN_DUAL_MODE
#include "wisun_dual.h"
#endif

extern void vcsystem_init(void);

int wisun_main_system_init(void)
{
    board_init();

    // RX LED ON
    hal_gpio_output_init(RADIO_HOST_RXLED_PORT, RADIO_HOST_RXLED_PIN);
    hal_gpio_low(RADIO_HOST_RXLED_PORT, RADIO_HOST_RXLED_PIN);

    // TX LED ON
    hal_gpio_output_init(RADIO_HOST_TXLED_PORT, RADIO_HOST_TXLED_PIN);
    hal_gpio_low(RADIO_HOST_TXLED_PORT, RADIO_HOST_TXLED_PIN);
    hal_misc_usec_delay(5000);

    // RX LED OFF
    hal_gpio_output_init(RADIO_HOST_RXLED_PORT, RADIO_HOST_RXLED_PIN);
    hal_gpio_high(RADIO_HOST_RXLED_PORT, RADIO_HOST_RXLED_PIN);

    // TX LED OFF
    hal_gpio_output_init(RADIO_HOST_TXLED_PORT, RADIO_HOST_TXLED_PIN);
    hal_gpio_high(RADIO_HOST_TXLED_PORT, RADIO_HOST_TXLED_PIN);

#if defined(CONFIG_WDT_EN)
    printf("Watchdog enabled\n");
    hal_wdt_set_period_2_secs();
    hal_wdt_enable();
#endif
    vcsystem_init();

    vcshell_init();

    extern void vcradio_alive_check(void);
    vcradio_alive_check();

    return 0;
}
