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

#if (CONFIG_RF_TEST_MODE==1)
#include "misc/ber.h"
#endif

#if (CONFIG_TRAV_TASK==1)
#include "misc/trav.h"
#endif

#if (CONFIG_DEBUG_PRCSSVNT==1)
#include "misc/prcssvnt.h"
#endif

#if (CONFIG_DEBUG_ROMCHK==1)
#include "misc/romchk.h"
#endif

#include "misc/topcmd.h"

#if (CONFIG_DEBUG_CENTRG==1)
#include "misc/centrgchk.h"
#endif

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

#if (CONFIG_DEBUG_MAIN_LOOP_STALL==1)
int m_main_loop_cnt=0;
#endif
int wisun_main_system_init(void)
{
    board_init();
#if CONFIG_WISUN_DUAL_MODE
    // RF TX LED ON
    hal_gpio_output_init(WISUN_DUAL_MODE_DEMO_RFTXLED_PORT, WISUN_DUAL_MODE_DEMO_RFTXLED_PIN);
    hal_gpio_low(WISUN_DUAL_MODE_DEMO_RFTXLED_PORT, WISUN_DUAL_MODE_DEMO_RFTXLED_PIN);

    // PLC TX LED ON
    hal_gpio_output_init(WISUN_DUAL_MODE_DEMO_PLCTXLED_PORT, WISUN_DUAL_MODE_DEMO_PLCTXLED_PIN);
    hal_gpio_low(WISUN_DUAL_MODE_DEMO_PLCTXLED_PORT, WISUN_DUAL_MODE_DEMO_PLCTXLED_PIN);
    hal_misc_usec_delay(5000);

    // RF TX LED OFF
    hal_gpio_output_init(WISUN_DUAL_MODE_DEMO_RFTXLED_PORT, WISUN_DUAL_MODE_DEMO_RFTXLED_PIN);
    hal_gpio_high(WISUN_DUAL_MODE_DEMO_RFTXLED_PORT, WISUN_DUAL_MODE_DEMO_RFTXLED_PIN);

    // PLC TX LED OFF
    hal_gpio_output_init(WISUN_DUAL_MODE_DEMO_PLCTXLED_PORT, WISUN_DUAL_MODE_DEMO_PLCTXLED_PIN);
    hal_gpio_high(WISUN_DUAL_MODE_DEMO_PLCTXLED_PORT, WISUN_DUAL_MODE_DEMO_PLCTXLED_PIN);
#else

#if !defined(MCU_USE_V8530)
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
#endif

#endif
#if defined(CONFIG_WDT_EN)
    printf("Watchdog enabled\n");
    hal_wdt_set_period_2_secs();
    hal_wdt_enable();
#endif
    vcsystem_init();

    vcshell_init();

    #if (CONFIG_TRAV_TASK==1)
        trav_init();
    #endif

#if (CONFIG_DEBUG_PRCSSVNT==1)
    prcssvnt_init();
#endif
    while (1) {
        #ifndef CONFIG_BUILD_WITH_VCRTOS
        uint8_t processList = 0;
        #endif
        do {
            #if (CONFIG_RF_TEST_MODE==1)
                if (ber_rf_test_mode()){
                    extern void vcshell_task(void);
                    ber_task();
                    #if (CONFIG_TRAV_TASK==1)
                        trav_task(TRAV_REASON_MAIN);
                    #endif
                    vcshell_task();
                    #if defined(CONFIG_WDT_EN)
                        hal_wdt_clear();
                    #endif
                }else{
                    #ifndef CONFIG_BUILD_WITH_VCRTOS
                    processList = process_run();
                    #endif
                }
            #else
                #ifndef CONFIG_BUILD_WITH_VCRTOS
                processList = process_run();
                #endif
            #endif
            #if (CONFIG_DEBUG_ROMCHK==1)
                rom_check();
            #endif
            #if (CONFIG_DEBUG_CENTRG==1)
                centrg_check();
            #endif
            #if (CONFIG_DEBUG_PHY_IRQ_STATUS_FIX==1)
                extern void phy_fix_irq_chk();
                phy_fix_irq_chk();
            #endif
            #if !defined(CONFIG_SYSTEM_bare) && defined(CONFIG_DEBUG_SEND_UDP_APP) && !defined(CONFIG_WISUN_ROOT)
                extern void udpsnd_task();
                udpsnd_task();
            #endif
            #if (CONFIG_DEBUG_MAIN_LOOP_STALL==1)
                m_main_loop_cnt++;
            #endif
                extern void vcradio_alive_check(void);
                vcradio_alive_check();
            #ifdef CONFIG_BUILD_WITH_VCRTOS
            thread_sleep();
            #endif
#ifdef CONFIG_BUILD_WITH_VCRTOS
        } while (1);
#else
        } while (processList > 0);
#endif
    }

    return 0;
}
