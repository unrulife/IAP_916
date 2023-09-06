#include "IAP_916.h"
#include "platform_api.h"
#include "eflash.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "bsp_usb.h"
//#include "bsp_usb_hid.h"
#include "bsp_usb_hid_iap.h"
#include "btstack_util.h"
#include "IAP_Transport.h"
#include "IAP_Application.h"
#include "IAP_Params.h"
#include "stdbool.h"
#include "rom_tools.h"
#include "IAP_Flash_WP.h"
#include "IAP_UserDef.h"
#include "crc16.h"


#if 1
#define IAP_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_ERROR(...)      
#endif

#if 1
#define IAP_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_DEBUG(...)      
#endif

// =================================================================================================


static void BootInit(void){
    IAP_ParamsInit();
}

static bool IsResetWhenUpgrading(void){
    IAP_FlashUpgradeInfo_t * upgrade_info = getUpgradeInfo();
    IAP_DEBUG("upgrade_flag: 0x%04X\n", upgrade_info->upgrade_sta);
    if (upgrade_info->upgrade_sta == IAP_UPGRADE_STA_FLAG_UNINIT || upgrade_info->upgrade_sta == IAP_UPGRADE_STA_FLAG_COMPLETE){
        return 0;
    }
    IAP_DEBUG("Reset when upgrading.\n");
    return 1;
}

static bool IsAPPNeedUpgrade(void){
    if (IAP_Params_app_upgrade_flag_get()){
        IAP_Params_app_upgrade_flag_clear(); // clear
        IAP_DEBUG("App need upgrade.\n");
        return 1;
    }
    IAP_DEBUG("App not need upgrade.\n");
    return 0;
}

static bool IsCombKeyAllPressed(void){
    IAP_DEBUG("Comb key not pressed.\n");
    return 0;
}

static bool IsAppCodeExist(void){
    if (!IsAppCodeEmpty()){
        IAP_DEBUG("APP exist.\n");
        return 1; // not empty, exist.
    }
    IAP_DEBUG("APP not exist.\n");
    return 0;
}

static void BootUpgradeStart(void){
    IAP_Flash_WP_Init();
    IAP_Application_Init();
    IAP_Transport_Init();
    bsp_usb_init();
}

static bool IsAppCheckSuccess(void){
    // check info exist ?
    IAP_FlashUpgradeInfo_t * upgrade_info = getUpgradeInfo();
    IAP_DEBUG("upgrade_flag: 0x%04X\n", upgrade_info->upgrade_sta);
    if (upgrade_info->upgrade_sta == IAP_UPGRADE_STA_FLAG_UNINIT){
        return 1; // check info not exist, jump directly.
    }
    // checksum header info.
    if (getCRC16((uint8_t *)&upgrade_info->header, sizeof(IAP_HeaderTypedef)) != upgrade_info->crcVal){
        IAP_ERROR("header check error.\n");
        return 0; // FAIL.
    }
    // check app code.
    uint8_t * pBinData   = (uint8_t *)APP_START_ADDR;
    uint16_t allBinCRC = getCRC16(pBinData, upgrade_info->allBinSize);
    IAP_DEBUG("allBinSize: 0x%X\n", upgrade_info->allBinSize);
    if(allBinCRC != upgrade_info->header.check.CRC){
        IAP_ERROR("[BOOT] error: =====>CRC CHECK: calc[0x%04x], recv[0x%04x]\n", allBinCRC, upgrade_info->header.check.CRC);
        return 0; // FAIL.
    }
    return 1; // SUCCESS.
}

void JumpToApp(uint32_t addr){
    IAP_DEBUG("JUMP TO APP [0x%08X]\n", addr);
#if USER_FLASH_LOCK_EN
    IAP_Flash_lock();
#endif
    Uart_Send_Complete_Check();
    for(int i=20000;i>0;i--);
    platform_switch_app(addr);
}



/**
 * @brief IAP_Init
 * 
 */
void IAP_Init(void){
    platform_printf("\n===>This is the BOOT code.\n");

    // IAP_ParamsTest();

    BootInit();
    if (IsResetWhenUpgrading()){
        BootUpgradeStart();
    } else if (IsAPPNeedUpgrade()){
        BootUpgradeStart();
    } else if (IsCombKeyAllPressed()){
        BootUpgradeStart();
    } else if (!IsAppCodeExist()){
        BootUpgradeStart();
    } else {
#if USER_APP_CODE_CHECK_CRC_EN
        if (!IsAppCheckSuccess()){
            BootUpgradeStart();
        } else {
            JumpToApp(APP_START_ADDR);
        }
#else 
        JumpToApp(APP_START_ADDR);
#endif
    }

    // IAP_Run();

}

