#ifndef _IAP_APPLICATION_H_
#define _IAP_APPLICATION_H_

#include <stdint.h>
#include "IAP_FLASH_MAP.H"

#define IAP_APP_MAX_BUFFER_SIZE         (30) //(8100) // MAX APP CMD
#define IAP_APP_RECV_CMD_MIN_SIZE       (5) // CMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_MIN_SIZE       (7) // CMD(1B) + ERRCODE(1B) + RSPCMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_PAYLOAD_OFFSET (5)

// ERROR code.
#define IAP_OK          (0)
#define IAP_FAIL        (1)

#define IAP_INVALID     (0)
#define IAP_VALID       (1)

// =================================================================================================
// CMD list
typedef enum{
    // HOST
    IAP_CMD_SWITCH_BOOT                 = 0xC0,

    // DEVICE
    IAP_CMD_ACK                         = 0xB0,

} IAP_APP_Cmd_t;

// error code.
typedef enum{
    IAP_APP_ERR_NONE                    = 0x00,
    IAP_APP_ERR_INVALID_CMD             = 0xE0,
    IAP_APP_ERR_LENGTH                  = 0xE1,
    IAP_APP_ERR_CRC                     = 0xE2,
    IAP_APP_ERR_BLOCK_NUM               = 0xE3,
    IAP_APP_ERR_BLOCK_SIZE              = 0xE4,
    IAP_APP_ERR_WR_OFFSET_ADDR          = 0xE5,
    IAP_APP_ERR_RD_OFFSET_ADDR          = 0xE6,
    IAP_APP_ERR_PARAM                   = 0xE7,
    IAP_APP_ERR_FLASH_OPERATE_FAIL      = 0xE8,
    IAP_APP_ERR_STATE_NOT_SATISFIED     = 0xE9,

} IAP_APP_ErrCode_t;

typedef struct __attribute__((packed)){
    uint8_t CMD;
    uint8_t errCode;
    uint8_t rspCmd;
    uint16_t length;
    uint16_t payload[1];
} IAP_APP_ACK_t;

typedef struct __attribute__((packed)){
    uint8_t CMD;
    uint16_t length;
    uint8_t payload[1];
} IAP_APP_cmd_t;


typedef struct __attribute__((packed)){
    // APP CMD control
    uint8_t rspCmd;
    uint8_t *buffer;
    uint16_t size;
    uint16_t payload_size;
    uint8_t *payload;

} IAP_APP_ctl_t;

// ===========================================================
#define IAP_CMD_SWITCH_BOOT_MAX_DELAY_MS    (1000)



// =================================================================================================

uint8_t * IAP_GetAppBuffer(void);
void IAP_Application_Init(void);

#endif


