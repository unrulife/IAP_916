#ifndef _IAP_PARAMS_H_
#define _IAP_PARAMS_H_

#include <stdint.h>
#include "IAP_FLASH_MAP.H"
#include "IAP_Application.h"

// upgrade status.
#define IAP_UPGRADE_STA_FLAG_UNINIT     0xFFFF
#define IAP_UPGRADE_STA_FLAG_RUNNING    0xFFAA
#define IAP_UPGRADE_STA_FLAG_COMPLETE   0x55AA

// version information.
#define IAP_VER_FLAG_UNINIT     0xFFFF
#define IAP_VER_FLAG_INITED     0x55AA

// upgrade info.
typedef struct __attribute__((packed)){
    uint16_t upgrade_sta;
    uint16_t crcVal;
    IAP_HeaderTypedef header;
} IAP_FlashUpgradeInfo_t;

// version info.
typedef struct __attribute__((packed)){
    uint16_t ver_flag;
    uint16_t crcVal;
    IAP_AppVerInfoTypedef verInfo;
} IAP_FlashVerInfo_t;

void IAP_ParamsTest(void);

#endif
