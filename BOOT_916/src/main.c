#include <stdio.h>
#include <string.h>
#include "profile.h"
#include "ingsoc.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "../data/setup_soc.cgen"
#include "iap_916.h"

static uint32_t cb_hard_fault(hard_fault_info_t *info, void *_)
{
    platform_printf("HARDFAULT:\nPC : 0x%08X\nLR : 0x%08X\nPSR: 0x%08X\n"
                    "R0 : 0x%08X\nR1 : 0x%08X\nR2 : 0x%08X\nP3 : 0x%08X\n"
                    "R12: 0x%08X\n",
                    info->pc, info->lr, info->psr,
                    info->r0, info->r1, info->r2, info->r3, info->r12);
    for (;;);
}

static uint32_t cb_assertion(assertion_info_t *info, void *_)
{
    platform_printf("[ASSERTION] @ %s:%d\n",
                    info->file_name,
                    info->line_no);
    for (;;);
}

static uint32_t cb_heap_out_of_mem(uint32_t tag, void *_)
{
    platform_printf("[OOM] @ %d\n", tag);
    for (;;);
}

#define PRINT_PORT    APB_UART0

uint32_t cb_putc(char *c, void *dummy)
{
    while (apUART_Check_TXFIFO_FULL(PRINT_PORT) == 1);
    UART_SendData(PRINT_PORT, (uint8_t)*c);
    return 0;
}

int fputc(int ch, FILE *f)
{
    cb_putc((char *)&ch, NULL);
    return ch;
}

void setup_peripherals(void)
{
    cube_setup_peripherals();
}

uint32_t on_lle_init(void *dummy, void *user_data)
{
    (void)(dummy);
    (void)(user_data);
    cube_on_lle_init();
    return 0;
}

trace_rtt_t trace_ctx = {0};


static const platform_evt_cb_table_t evt_cb_table =
{
    .callbacks = {
        [PLATFORM_CB_EVT_HARD_FAULT] = {
            .f = (f_platform_evt_cb)cb_hard_fault,
        },
        [PLATFORM_CB_EVT_ASSERTION] = {
            .f = (f_platform_evt_cb)cb_assertion,
        },
        [PLATFORM_CB_EVT_HEAP_OOM] = {
            .f = (f_platform_evt_cb)cb_heap_out_of_mem,
        },
        [PLATFORM_CB_EVT_PROFILE_INIT] = {
            .f = setup_profile,
        },
        [PLATFORM_CB_EVT_LLE_INIT] = {
            .f = on_lle_init,
        },
        [PLATFORM_CB_EVT_PUTC] = {
            .f = (f_platform_evt_cb)cb_putc,
        },
        [PLATFORM_CB_EVT_TRACE] = {
            .f = (f_platform_evt_cb)cb_trace_rtt,
            .user_data = &trace_ctx,
        },
    }
};

void Uart_Send_Complete_Check(void){
    while (apUART_Check_TXFIFO_EMPTY(PRINT_PORT) == 0);
}

int app_main()
{
    cube_soc_init();

    // setup event handlers
    platform_set_evt_callback_table(&evt_cb_table);

    setup_peripherals();
    SYSCTRL_Init();
    IAP_Init();

    trace_rtt_init(&trace_ctx);
    // TODO: config trace mask
    platform_config(PLATFORM_CFG_TRACE_MASK, 0);

    return 0;
}

