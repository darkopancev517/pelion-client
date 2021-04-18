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

PROCESS_NAME(shell_app_process);

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

extern unsigned short node_id;
extern unsigned char node_mac[8];
extern unsigned short random_seed;

extern void uip_debug_ipaddr_print(const uip_ipaddr_t *addr);
extern int rpl_ns_num_nodes(void);

/* The table is used for default setting when
   1. candidate parent select mode is 1 or
   2. adaptive cca mode is 4.                 */
int threshold_param_table[THRESHOLD_PARAM_RROFILE_TOTOL][THRESHOLD_PARAM_ITEM_TOTAL] = {
    //RPL_TH_H    RPL_TH    RPL_TH_H    DEV_SEN    CCA_TH    NOISE_FLOOR
    {      -95,      -98,       -101,      -105,     -113,          -117},   //THRESHOLD_PARAM_PROFILE_RATE_50K
    {      -89,      -92,        -95,       -99,     -107,          -113},   //THRESHOLD_PARAM_PROFILE_RATE_100K
    {      -88,      -91,        -94,       -98,     -106,          -112},   //THRESHOLD_PARAM_PROFILE_RATE_150K
    {      -85,      -88,        -91,       -95,     -103,          -110}    //THRESHOLD_PARAM_PROFILE_RATE_300K
};

#if CONFIG_CONTIKI_WITH_ETHERNET && CONFIG_BOARD_vc6328 && CONFIG_WISUN_DUAL_MODE && CONFIG_WISUN_ROOT
/*	Actival ethernet for dualmode root on vc6328 */
PROCESS_NAME(ethernet_process);
extern void ethernet_tx(const char *data, unsigned int len);
static int fallback_output(void)
{
    ethernet_tx(&uip_buf[UIP_LLH_LEN], uip_len);
    return uip_len;
}
#endif

unsigned int vc_app_init()
{
    unsigned int retVal = 0;

#if CONFIG_DRV_APPUART_VC_SNIFFER
    extern void vc_sniffer_init(void);
    vc_sniffer_init();
#endif

#if CONFIG_DRV_APPUART_VERTEXCOM_DLMS
    dlms_init(NULL, 0);
#endif

#if CONFIG_DRV_APPUART_ATCMD
    extern void atcmd_uart_init(recv_handler_cb recv_fx, unsigned int data_position);
    atcmd_uart_init(NULL, 0);
#endif

#if CONFIG_DRV_APPUART_VC_TIMERUART
    vc_timerUart_init(NULL, 0);
#endif

#if CONFIG_DRV_APPUART_VERTEXCOM_SLIP
    slip_init(NULL, 0);
#endif

#if CONFIG_DRV_APPUART_VC_QGDW_1376
    vc_qgdw_init(NULL, 0);
#endif

    extern int uart_meter_shell_init(void);
    uart_meter_shell_init();

#if CONFIG_CONTIKI_WITH_COAP
   retVal = vc_app_startCoapHandler();
   if (0 != retVal) {
       printf("Error:%u\n",retVal);
       goto err_handler;
   }
#endif

#if CONFIG_CONTIKI_WITH_MW_UNITTEST
    vc_sysResAL_unitTestInit();
#endif

#if  CONFIG_CONTIKI_WITH_COAP
err_handler:
#endif
    return retVal;
}

void set_node_addr(void)
{
    linkaddr_t addr;

    memset(&addr, 0, sizeof(linkaddr_t));
    memcpy(addr.u8, node_mac, sizeof(addr.u8));
    linkaddr_set_node_addr(&addr);
#if NETSTACK_CONF_WITH_IPV6
    memcpy(&uip_lladdr.addr, node_mac, sizeof(uip_lladdr.addr));
#endif
    random_init(random_seed ? random_seed : node_id);
}

void print_info(void)
{
    printf("--------------------------------------------------------\r\n");
    printf("VERTEXCOM Technologies, Inc.\r\n");
    printf("--------------------------------------------------------\r\n");
    printf("NET       : %s\r\n", NETSTACK_NETWORK.name);
    printf("LLSEC     : %s\r\n", NETSTACK_LLSEC.name);
    printf("MAC       : %s\r\n", NETSTACK_MAC.name);
    printf("RDC       : %s\r\n", NETSTACK_RDC.name);
	printf("SW BRANCH : %s\r\n",STR(SW_BRANCH));
    printf("NODE ID   : 0x%04x\r\n", node_id);
    printf("NODE MAC  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \r\n",
           node_mac[0],node_mac[1],node_mac[2],node_mac[3],
           node_mac[4],node_mac[5],node_mac[6],node_mac[7]);

#if defined(MCU_USE_V8530)
    extern uint32_t get_bootloader_ver(void);
    printf("BOOTLOADER: %02"PRIx32"\r\n", get_bootloader_ver());
#endif

    printf("--------------------------------------------------------\r\n");
}

void print_processes(struct process *const processes[])
{
    printf("Starting:");

    while (*processes != NULL) {
#ifdef CONFIG_BUILD_WITH_VCRTOS
        printf(" '%s'", (*processes)->process_name);
#else
        printf(" '%s'", (*processes)->name);
#endif
        processes++;
    }

    printf("\r\n");
    printf("\r\n");
}

extern void vcShellContikiIfaceInit(void);
uint8_t vc_net_service_init_done = 0;
uint8_t vc_net_rpl_init_done = 0;
extern uint8_t wisun_mac_rpl_dhcps_flag;
void vc_net_rpl_init(void)
{
    if (vc_net_rpl_init_done++) {
        printf("%s fail, do not init again\n", __FUNCTION__);
        return;
    }

    if (CIU_CONFIG.ciu_mode == 0 || linkaddr_cmp((linkaddr_t *)CIU_CONFIG.ciu_eui64, (linkaddr_t *)node_mac) == 0) {
#if UIP_CONF_IPV6_RPL
        rpl_init();
#endif
#ifdef WISUN_ROOT
        rpl_root_init();
#endif
        wisun_rpl_nd6_init();
        wisun_mac_rpl_dhcps_flag = 1;
    }
}

void vc_net_service_init(void)
{
    if (vc_net_service_init_done++) {
        printf("%s fail, do not init again\n", __FUNCTION__);
        return;
    }

    printf("Net Service Start:\n");
    printf("RAND INIT : 0x%04x\n", random_seed);
    printf("NODE ID   : 0x%04x\n", node_id);
    printf("NODE MAC  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
           node_mac[0],node_mac[1],node_mac[2],node_mac[3],
           node_mac[4],node_mac[5],node_mac[6],node_mac[7]);
    set_node_addr();
#if NETSTACK_CONF_WITH_IPV6
    process_start(&tcpip_process, NULL);
#endif
    netstack_init();
#if UIP_CONF_TCP
    telnetd_init();
#endif
#if CONFIG_CONTIKI_WITH_ETHERNET && CONFIG_BOARD_vc6328 && CONFIG_WISUN_DUAL_MODE && CONFIG_WISUN_ROOT
	/*	Actival ethernet for dualmode root on vc6328 */
	SYS_HOOK.uip_fallback_if.output = fallback_output;
	process_start(&ethernet_process, NULL);
#endif
    vc_app_init();

    if (SYS_HOOK.after_netstack_init_fn) {
        SYS_HOOK.after_netstack_init_fn();
    }
}

void vcsystem_init(void)
{
    node_id_restore(); // restore default node if and mac from HW NVRAM

#ifdef CONFIG_BUILD_WITH_VCRTOS
    process_init(instance_get());
#else
    process_init();
#endif

    extern void shell_init(void);
    shell_init();

    process_start(&etimer_process, NULL);
    ctimer_init();
    extern int log_output_init(uint8_t uart_port, uint32_t baudrate, uint16_t size, uint8_t num);
    log_output_init(CONFIG_BOARD_DBG_UART_INDEX,115200,PARAM(CONFIG_DBG_UART_TXDMA_BUF_SIZE_addr),PARAM(CONFIG_DBG_UART_TXDMA_BUF_NUM_addr));

    queuebuf_init();
#if CONFIG_DRV_APPUART
    extern void uart_meter_arch_init(void);
    uart_meter_arch_init();
#endif

    if (SYS_HOOK.before_netstack_init_fn) {
        SYS_HOOK.before_netstack_init_fn();
    }
#if CONFIG_DRV_APPUART_ATCMD
    extern void atcmd_init(void);
    atcmd_init();
#endif
#ifdef USE_CONFIG_TOOL
    extern void config_tool_init(void);
    config_tool_init(); // shell related init
#endif

    print_info();

    vcShellContikiIfaceInit();

#if (CONFIG_TRAV_TASK==1)
    extern void trav_process_init(void);
    trav_process_init();
#endif

#if (CONFIG_CONTIKI_SQ_TASK==1)
    extern void sq_process_init(void);
    sq_process_init();
#endif

    topcmd_process_init();

    process_start(&shell_app_process, NULL);


    if (SYS_CONFIG.auto_start)
        vc_net_service_init();

    print_processes(autostart_processes);
    autostart_start(autostart_processes);

    if (SYS_CONFIG.auto_start) {
        vc_net_rpl_init();
    }
}
