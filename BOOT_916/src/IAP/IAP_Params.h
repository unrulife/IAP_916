#ifndef _IAP_PARAMS_H_
#define _IAP_PARAMS_H_

#include <stdint.h>
#include "stdbool.h"
#include "IAP_FLASH_MAP.H"
#include "IAP_Application.h"

// upgrade status.
#define IAP_UPGRADE_STA_FLAG_UNINIT     0xFFFF
#define IAP_UPGRADE_STA_FLAG_RUNNING    0xFFAA
#define IAP_UPGRADE_STA_FLAG_COMPLETE   0x55AA

// version information.
#define IAP_VER_FLAG_UNINIT     0xFFFF
#define IAP_VER_FLAG_INITED     0x55AA

#define BIT_0    (1<<0)
#define BIT_1    (1<<1)

// version info type
#define IAP_PARAMS_VER_FLAG                 1
#define IAP_PARAMS_VER_CRC                  2
#define IAP_PARAMS_VER_VERSION              3
// version info OFFSET
#define IAP_PARAMS_VER_FLAG_OFFSET              0
#define IAP_PARAMS_VER_CRC_OFFSET               2
#define IAP_PARAMS_VER_VERSION_OFFSET           4


// upgrade info type
#define IAP_PARAMS_HEADER                   1
#define IAP_PARAMS_ALL_BIN_SIZE             2
#define IAP_PARAMS_CRC_VAL                  3
#define IAP_PARAMS_UPGRADE_FLAG_LOW         4
#define IAP_PARAMS_UPGRADE_FLAG_HIGH        5

// upgrade info OFFSET
#define IAP_PARAMS_HEADER_OFFSET                8
#define IAP_PARAMS_ALL_BIN_SIZE_OFFSET          4
#define IAP_PARAMS_CRC_VAL_OFFSET               2
#define IAP_PARAMS_UPGRADE_FLAG_LOW_OFFSET      0
#define IAP_PARAMS_UPGRADE_FLAG_HIGH_OFFSET     1

// upgrade info.
typedef struct __attribute__((packed)){
    union{
        uint16_t upgrade_sta;
        struct{
            uint8_t upgrade_sta_low; //first storage byte in flash.
            uint8_t upgrade_sta_high;
        };
    };
    uint16_t crcVal;
    uint32_t allBinSize;
    IAP_HeaderTypedef header;
} IAP_FlashUpgradeInfo_t;

// version info.
typedef struct __attribute__((packed)){
    uint16_t ver_flag;
    uint16_t crcVal;
    IAP_AppVerInfoTypedef verInfo;
} IAP_FlashVerInfo_t;

IAP_FlashUpgradeInfo_t * getUpgradeInfo(void);
IAP_FlashVerInfo_t * getVersionInfo(void);
void IAP_ParamsInit(void);
void IAP_Params_update_Header_ToFlash(IAP_HeaderTypedef * header);
void IAP_Params_update_AllBinSize_ToFlash(uint32_t size);

// ------------------------
void IAP_ParamsTest(void);



bool IAP_Params_app_upgrade_flag_get(void);
void IAP_Params_app_upgrade_flag_clear(void);
void IAP_Params_app_upgrade_flag_set(void);


bool IsAppCodeEmpty(void);


void AppJumpToBoot(void);

#endif
