#include "string.h"
#include "IAP_Application.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_usb_hid_iap.h"
#include "btstack_util.h"
#include "IAP_Transport.h"
#include "eflash.h"
#include "rom_tools.h"
#include "IAP_Params.h"

#if 1
#define IAP_PARAM_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_PARAM_ERROR(...)      
#endif

#if 1
#define IAP_PARAM_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_PARAM_DEBUG(...)      
#endif

// =================================================================================================
#define IAP_FLASH_UPGRADE_INFO_ADDR     (BOOT_PARAM_ADDR)
#define IAP_FLASH_VERSION_INFO_ADDR     (BOOT_PARAM_ADDR + EFLASH_ERASABLE_SIZE)


static IAP_FlashUpgradeInfo_t upgradeInfo;
static IAP_FlashVerInfo_t versionInfo;


// =================================================================================================

// upgrade info.
static uint32_t IAP_FlashUpgradeInfo_Erase(void){
    uint32_t err = erase_flash_sector(IAP_FLASH_UPGRADE_INFO_ADDR);
    if(err){
        IAP_PARAM_ERROR("[PARAM] error: erase [%d], ADDR:0x%08X\n", err, IAP_FLASH_UPGRADE_INFO_ADDR);
        return IAP_FAIL;
    }
    return IAP_OK;
}

static uint32_t IAP_FlashUpgradeInfo_UpdateToFlash(void){
    uint32_t err = write_flash(IAP_FLASH_UPGRADE_INFO_ADDR, (uint8_t *)&upgradeInfo, sizeof(IAP_FlashUpgradeInfo_t));
    if(err){
        IAP_PARAM_ERROR("[PARAM] error: write [%d]\n", err);
        return IAP_FAIL;
    }
    return IAP_OK;
}

static uint32_t IAP_FlashUpgradeInfo_ReadToStu(void){
    uint8_t * pData = (uint8_t *)(IAP_FLASH_UPGRADE_INFO_ADDR);
    memcpy((uint8_t *)&upgradeInfo, pData, sizeof(IAP_FlashUpgradeInfo_t));
    return IAP_OK;
}


// version info.
static uint32_t IAP_FlashVersionInfo_Erase(void){
    uint32_t err = erase_flash_sector(IAP_FLASH_VERSION_INFO_ADDR);
    if(err){
        IAP_PARAM_ERROR("[PARAM] error: erase [%d], ADDR:0x%08X\n", err, IAP_FLASH_VERSION_INFO_ADDR);
        return IAP_FAIL;
    }
    return IAP_OK;
}

static uint32_t IAP_FlashVersionInfo_UpdateToFlash(void){
    uint32_t err = write_flash(IAP_FLASH_VERSION_INFO_ADDR, (uint8_t *)&versionInfo, sizeof(IAP_FlashVerInfo_t));
    if(err){
        IAP_PARAM_ERROR("[PARAM] error: write [%d], ADDR:0x%08X, SIZE:%d\n", err, IAP_FLASH_VERSION_INFO_ADDR, sizeof(IAP_FlashVerInfo_t));
        return IAP_FAIL;
    }
    return IAP_OK;
}

static uint32_t IAP_FlashVersionInfo_ReadToStu(void){
    uint8_t * pData = (uint8_t *)(IAP_FLASH_VERSION_INFO_ADDR);
    memcpy((uint8_t *)&versionInfo, pData, sizeof(IAP_FlashVerInfo_t));
    return IAP_OK;
}

static void PrintUpgradeInfo(void){
    IAP_PARAM_DEBUG("\n------------------------------------\n");
    IAP_PARAM_DEBUG("upgrade_sta = 0x%04X\n", upgradeInfo.upgrade_sta);
    IAP_PARAM_DEBUG("crcVal = 0x%04X\n", upgradeInfo.crcVal);
    IAP_PARAM_DEBUG("header: \n");
    uint8_t *pData = (uint8_t *)&upgradeInfo.header;
    printf_hexdump(pData, sizeof(IAP_HeaderTypedef));
    IAP_PARAM_DEBUG("\n");
}

static void PrintVersionInfo(void){
    IAP_PARAM_DEBUG("\n============\n");
    IAP_PARAM_DEBUG("ver_flag = 0x%04X\n", versionInfo.ver_flag);
    IAP_PARAM_DEBUG("crcVal = 0x%04X\n", versionInfo.crcVal);
    IAP_PARAM_DEBUG("verInfo: \n");
    uint8_t *pData = (uint8_t *)&versionInfo.verInfo;
    printf_hexdump(pData, sizeof(IAP_AppVerInfoTypedef));
    IAP_PARAM_DEBUG("\n");
}

void IAP_ParamsTest(void){
    // upgrade info.
    IAP_FlashUpgradeInfo_Erase();
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    upgradeInfo.upgrade_sta = 0x55AA;
    upgradeInfo.crcVal = 0x1234;
    PrintUpgradeInfo();
    IAP_FlashUpgradeInfo_UpdateToFlash();

    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    upgradeInfo.upgrade_sta = 0x0000;
    IAP_FlashUpgradeInfo_UpdateToFlash();

    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    // version info.

}





// =================================================================================================
