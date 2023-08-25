#include "string.h"
#include "IAP_Transport.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_usb_hid_iap.h"
#include "btstack_util.h"
#include "IAP_Transport.h"
#include "eflash.h"
#include "rom_tools.h"

#if 1
#define IAP_APP_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_ERROR(...)      
#endif

#if 1
#define IAP_APP_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_DEBUG(...)      
#endif

typedef struct __attribute__((packed)){
    uint8_t CMD;
    uint16_t length;
    uint8_t errCode;
    uint16_t crcValue;
} IAP_APP_ACK_t;

static void IAP_APP_SendRSP(uint8_t cmd, uint8_t err){
    IAP_APP_ACK_t ACK;
    ACK.CMD      = cmd;
    ACK.length   = 0x0001;
    ACK.errCode  = err;
    ACK.crcValue = crc((uint8_t *)&ACK, 4);
#if 1
    IAP_APP_DEBUG("APP SEND[%d]: ", sizeof(IAP_APP_ACK_t));
    printf_hexdump((uint8_t *)&ACK, sizeof(IAP_APP_ACK_t));
#endif
    IAP_Transport_send_single_pack((uint8_t *)&ACK, sizeof(IAP_APP_ACK_t));
}

static void IAP_transport_recv_cmd_callback(uint8_t *data, uint16_t len){
    platform_printf("\n\n========>>>>>APP RECV[%d]: ", len); printf_hexdump(data, len);
    

    IAP_APP_SendRSP(0xA0, 0x00);
}


void IAP_Application_Init(void){
    IAP_Transport_recv_cmd_callback_register(IAP_transport_recv_cmd_callback);
}


