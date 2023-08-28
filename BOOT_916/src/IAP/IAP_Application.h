#ifndef _IAP_APPLICATION_H_
#define _IAP_APPLICATION_H_

#include <stdint.h>
#include "IAP_FLASH_MAP.H"

#define IAP_APP_MAX_BUFFER_SIZE         (8100)
#define IAP_APP_RECV_CMD_MIN_SIZE       (5) // CMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_MIN_SIZE       (7) // CMD(1B) + ERRCODE(1B) + RSPCMD(1B) + LEN(2B) + CRC(2B)
#define IAP_APP_SEND_CMD_PAYLOAD_OFFSET (5)

// valid check code.
#define IAP_INVALID     (0)
#define IAP_VALID       (1)

// --------------------------------------------------------------------------------
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
    IAP_APP_ERR_STATE_NOT_SATISFIED     = 0xE9,

    IAP_APP_ERR_HEADER_UPGRADE_FLAG     = 0xF0,
    IAP_APP_ERR_HEADER_CHIP_ID          = 0xF1,
    IAP_APP_ERR_HEADER_ITEM_IINFO       = 0xF2,
    IAP_APP_ERR_HEADER_HARDWARE_VER     = 0xF3,
    IAP_APP_ERR_HEADER_SOFTWARE_VER     = 0xF4,
    IAP_APP_ERR_HEADER_CHECK_INFO       = 0xF5,
    IAP_APP_ERR_HEADER_BLOCK_INFO       = 0xF6,
    IAP_APP_ERR_HEADER_UPGRADE_TYPE     = 0xF7,
    IAP_APP_ERR_HEADER_ENCRYPT          = 0xF8,

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

    uint8_t upgrade_flag;
} IAP_APP_ctl_t;



// =================================================================================================
// USER CONFIG BEGIN ------------------
// supported upgrade type.
#define USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_APP_ONLY_EN               (1) // APP only.
#define USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_APP_EN           (0) // platform+app
#define USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_BOOT_EN          (0) // platform+boot
#define USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_BOOT_APP_EN      (0) // platform+boot+app
// supported check method.
#define USER_CFG_IAP_CHECK_TYPE_SUPPORT_CRC_EN                      (1) // CRC
#define USER_CFG_IAP_CHECK_TYPE_SUPPORT_SUM_EN                      (0) // SUM
// supported encrypt method.
#define USER_CFG_IAP_ENCRYPT_TYPE_SUPPORT_XOR_EN                    (0) // XOR
#define USER_CFG_IAP_ENCRYPT_TYPE_SUPPORT_AES_EN                    (0) // AES128
// USER CONFIG END ------------------


// support check type.
#define IAP_CHECK_TYPE_CRC                  (0x00)
#define IAP_CHECK_TYPE_SUM                  (0x01)

// block size
#define IAP_MAX_BLOCK_SIZE                  (8192)
#define IAP_MIN_BLOCK_SIZE                  (12)

// upgrade type
#define IAP_UPGRADE_TYPE_APP_ONLY           (0x00)
#define IAP_UPGRADE_TYPE_PLATFORM_APP       (0x01)
#define IAP_UPGRADE_TYPE_PLATFORM_BOOT      (0x02)
#define IAP_UPGRADE_TYPE_PLAT_BOOT_APP      (0x03)

// encrypt type
#define IAP_ENCRYPT_TYPE_XOR                (0x00)
#define IAP_ENCRYPT_TYPE_AES128             (0x01)

// encrypt len
#define IAP_ENCRYPT_LEN_XOR                 (16)
#define IAP_ENCRYPT_LEN_AES128              (32)

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
	unsigned char key[16];
    unsigned char iv[16];
}IAP_EncryptInfoTypedef;


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
    IAP_EncryptInfoTypedef encrypt;
    uint8_t reserved[22];
	
}IAP_HeaderTypedef;



// =================================================================================================

uint8_t * IAP_GetAppBuffer(void);
void IAP_Application_Init(void);

#endif


