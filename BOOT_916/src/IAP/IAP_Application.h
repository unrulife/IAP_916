#ifndef _IAP_APPLICATION_H_
#define _IAP_APPLICATION_H_

#include <stdint.h>

#define IAP_APP_MAX_BUFFER_SIZE         (8100)
#define IAP_APP_RECV_CMD_MIN_SIZE       (5) // CMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_MIN_SIZE       (7) // CMD(1B) + ERRCODE(1B) + RSPCMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_PAYLOAD_OFFSET (5)

// CMD list
typedef enum{
    // HOST
    IAP_CMD_START                       = 0xA0,
    IAP_CMD_FLASH_WRITE                 = 0xA1,
    IAP_CMD_FLASH_READ                  = 0xA2,
    IAP_CMD_REBOOT                      = 0xA3,
    IAP_CMD_SWITCH_APP                  = 0xA4,

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
    uint8_t rspCmd;
    uint8_t *buffer;
    uint16_t size;
    uint16_t payload_size;
    uint8_t *payload;
} IAP_APP_ctl_t;



// =================================================================================================

typedef struct 
{
	unsigned char len;
	unsigned char str[15];
}IAP_ChipIDTypedef;

typedef struct 
{
	unsigned char len;
	unsigned char str[23];
}IAP_ItemInfoTypedef;

// CHECK
typedef struct 
{
    unsigned char type;
    unsigned char len;
    union{
        unsigned short CRC;
        unsigned short SUM;
    }val;
	unsigned char data[2];
}IAP_CheckInfoTypedef;

// BLOCK
typedef struct 
{
    unsigned short size;
    unsigned short num;
}IAP_BlockInfoTypedef;

// 加密信息
typedef struct 
{
    unsigned char enable;
    unsigned char type;
    unsigned char len;
	unsigned char val[16];
    unsigned char iv[16];
}IAP_KeyInfoTypedef;


/* OTA 128 bytes header information struct */
typedef struct 
{
	uint8_t upgradeFlag[8];
    IAP_ChipIDTypedef chip_id;
    IAP_ItemInfoTypedef item_info;
    uint8_t HW[6];
    uint8_t SW[6];
    IAP_CheckInfoTypedef check;
    IAP_BlockInfoTypedef block;
    uint8_t  upgradeType;
    IAP_KeyInfoTypedef key;
    uint8_t reserved[22];
	
}IAP_HeaderTypedef;



// =================================================================================================

uint8_t * IAP_GetAppBuffer(void);
void IAP_Application_Init(void);

#endif


