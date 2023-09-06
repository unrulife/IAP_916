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
#include "IAP_UserDef.h"
#include "crc16.h"

#if USER_IAP_PARAM_ERROR_LOG_EN
#define IAP_PARAM_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_PARAM_ERROR(...)      
#endif

#if USER_IAP_PARAM_DEBUG_LOG_EN
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

static uint32_t IAP_FlashUpgradeInfo_UpdateToFlash(uint32_t offset, uint8_t *data, uint16_t len){
    uint32_t err = write_flash(IAP_FLASH_UPGRADE_INFO_ADDR + offset, (uint8_t *)data, len);
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

static uint32_t IAP_FlashVersionInfo_UpdateToFlash(uint32_t offset, uint8_t *data, uint16_t len){
    uint32_t err = write_flash(IAP_FLASH_VERSION_INFO_ADDR + offset, (uint8_t *)data, len);
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
#if USER_IAP_PARAM_DEBUG_LOG_EN
    IAP_PARAM_DEBUG("\n------------------------------------\n");
    IAP_PARAM_DEBUG("upgrade_sta = 0x%04X\n", upgradeInfo.upgrade_sta);
    IAP_PARAM_DEBUG("upgrade_sta1 = 0x%02X\n", upgradeInfo.upgrade_sta_low);
    IAP_PARAM_DEBUG("upgrade_sta2 = 0x%02X\n", upgradeInfo.upgrade_sta_high);
    IAP_PARAM_DEBUG("crcVal = 0x%04X\n", upgradeInfo.crcVal);
    IAP_PARAM_DEBUG("allBinSize = 0x%04X\n", upgradeInfo.allBinSize);
    IAP_PARAM_DEBUG("header: \n");
    uint8_t *pData = (uint8_t *)&upgradeInfo.header;
    printf_hexdump(pData, sizeof(IAP_HeaderTypedef));
    IAP_PARAM_DEBUG("\n");
#endif
}

static void PrintVersionInfo(void){
#if USER_IAP_PARAM_DEBUG_LOG_EN
    IAP_PARAM_DEBUG("\n============\n");
    IAP_PARAM_DEBUG("ver_flag = 0x%04X\n", versionInfo.ver_flag);
    IAP_PARAM_DEBUG("crcVal = 0x%04X\n", versionInfo.crcVal);
    IAP_PARAM_DEBUG("verInfo: \n");
    uint8_t *pData = (uint8_t *)&versionInfo.verInfo;
    printf_hexdump(pData, sizeof(IAP_AppVerInfoTypedef));
    IAP_PARAM_DEBUG("\n");
#endif
}

IAP_FlashUpgradeInfo_t * getUpgradeInfo(void){
    return &upgradeInfo;
}

IAP_FlashVerInfo_t * getVersionInfo(void){
    return &versionInfo;
}

void IAP_ParamsInit(void){

#if USER_ERASE_BOOT_PARAM_EN
    IAP_FlashUpgradeInfo_Erase();
    IAP_FlashVersionInfo_Erase();
#endif

    IAP_FlashUpgradeInfo_ReadToStu();
    IAP_FlashVersionInfo_ReadToStu();
}

static void IAP_Params_UpgradeInfoUpdates(uint8_t TYPE, uint8_t *data, uint16_t len){
    switch(TYPE){
        case IAP_PARAMS_HEADER:
            IAP_FlashUpgradeInfo_UpdateToFlash(IAP_PARAMS_HEADER_OFFSET, data, len);
            break;
        case IAP_PARAMS_ALL_BIN_SIZE:
            IAP_FlashUpgradeInfo_UpdateToFlash(IAP_PARAMS_ALL_BIN_SIZE_OFFSET, data, len);
            break;
        case IAP_PARAMS_CRC_VAL:
            IAP_FlashUpgradeInfo_UpdateToFlash(IAP_PARAMS_CRC_VAL_OFFSET, data, len);
            break;
        case IAP_PARAMS_UPGRADE_FLAG_LOW:
            IAP_FlashUpgradeInfo_UpdateToFlash(IAP_PARAMS_UPGRADE_FLAG_LOW_OFFSET, data, len);
            break;
        case IAP_PARAMS_UPGRADE_FLAG_HIGH:
            IAP_FlashUpgradeInfo_UpdateToFlash(IAP_PARAMS_UPGRADE_FLAG_HIGH_OFFSET, data, len);
            break;
    }
}

static void IAP_Params_VersionInfoUpdates(uint8_t TYPE, uint8_t *data, uint16_t len){
    switch(TYPE){
        case IAP_PARAMS_VER_FLAG:
            IAP_FlashVersionInfo_UpdateToFlash(IAP_PARAMS_VER_FLAG_OFFSET, data, len);
            break;
        case IAP_PARAMS_VER_CRC:
            IAP_FlashVersionInfo_UpdateToFlash(IAP_PARAMS_VER_CRC_OFFSET, data, len);
            break;
        case IAP_PARAMS_VER_VERSION:
            IAP_FlashVersionInfo_UpdateToFlash(IAP_PARAMS_VER_VERSION_OFFSET, data, len);
            break;
    }
}

void IAP_Params_update_Header_ToFlash(IAP_HeaderTypedef * header){
    IAP_PARAM_DEBUG("update header info to boot params flash.\n");
    // erase upgrade info area.
    IAP_FlashUpgradeInfo_Erase();
    // write header.
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_HEADER, (uint8_t *)header, sizeof(IAP_HeaderTypedef));
    // write crc.
    upgradeInfo.crcVal = getCRC16((uint8_t *)header, sizeof(IAP_HeaderTypedef));
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_CRC_VAL, (uint8_t *)&upgradeInfo.crcVal, 2);
    // update upgrade flag to updating status. [FFAA]
    upgradeInfo.upgrade_sta = 0x55AA;
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_UPGRADE_FLAG_LOW, (uint8_t *)&upgradeInfo.upgrade_sta_low, 1);
    // read info.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();
    PrintVersionInfo();
}

static void IAP_Params_update_versionInfo_ToFlash(void){
    // erase version area.
    IAP_FlashVersionInfo_Erase();
    // write version.
    IAP_FlashUpgradeInfo_ReadToStu();
    IAP_FlashVersionInfo_ReadToStu();
    memcpy((uint8_t *)&versionInfo.verInfo, (uint8_t *)&upgradeInfo.header.verInfo, sizeof(IAP_AppVerInfoTypedef));
    IAP_Params_VersionInfoUpdates(IAP_PARAMS_VER_VERSION, (uint8_t *)&versionInfo.verInfo, sizeof(IAP_AppVerInfoTypedef));
    // write crc.
    versionInfo.crcVal = getCRC16((uint8_t *)&versionInfo.verInfo, sizeof(IAP_AppVerInfoTypedef));
    IAP_PARAM_DEBUG("wr crc: 0x%04X.\n", versionInfo.crcVal);
    IAP_Params_VersionInfoUpdates(IAP_PARAMS_VER_CRC, (uint8_t *)&versionInfo.crcVal, 2);
    // write flag.
    versionInfo.ver_flag = 0x55AA;
    IAP_Params_VersionInfoUpdates(IAP_PARAMS_VER_FLAG, (uint8_t *)&versionInfo.ver_flag, 2);
    // read version info.
    IAP_FlashVersionInfo_ReadToStu();
}

void IAP_Params_update_AllBinSize_ToFlash(uint32_t size){
    IAP_PARAM_DEBUG("update allBinSize to boot params flash.\n");
    // write all bin size
    upgradeInfo.allBinSize = size;
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_ALL_BIN_SIZE, (uint8_t *)&upgradeInfo.allBinSize, 4);
    // update version information.
    IAP_Params_update_versionInfo_ToFlash();
    // update upgrade flag to complete status. [55AA]
    upgradeInfo.upgrade_sta = 0x55AA; 
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_UPGRADE_FLAG_HIGH, (uint8_t *)&upgradeInfo.upgrade_sta_high, 1);
    // read info.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();
    PrintVersionInfo();
}

// --------------------------------------------------------------
void IAP_ParamsTest(void){

    // upgrade info.
    IAP_FlashUpgradeInfo_Erase();

    // read.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    // wr.
    upgradeInfo.upgrade_sta = 0x55AA;
    // upgradeInfo.crcVal = 0x1234;
    PrintUpgradeInfo();
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_UPGRADE_FLAG_LOW, (uint8_t *)&upgradeInfo.upgrade_sta_low, 1);

    // read.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    // wr.
    upgradeInfo.upgrade_sta = 0x55AA;
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_UPGRADE_FLAG_HIGH, (uint8_t *)&upgradeInfo.upgrade_sta_high, 1);

    // read.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    // wr.
    upgradeInfo.crcVal = 0x1122;
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_CRC_VAL, (uint8_t *)&upgradeInfo.crcVal, 2);

    // read.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

    // wr.
    upgradeInfo.allBinSize = 0x5678;
    IAP_Params_UpgradeInfoUpdates(IAP_PARAMS_ALL_BIN_SIZE, (uint8_t *)&upgradeInfo.allBinSize, 4);

    // read.
    IAP_FlashUpgradeInfo_ReadToStu();
    PrintUpgradeInfo();

}





// =================================================================================================

bool IAP_Params_app_upgrade_flag_get(void){
    if (platform_read_persistent_reg() & BIT_1){
        return 1;
    }
    return 0;
}

void IAP_Params_app_upgrade_flag_clear(void){
    platform_write_persistent_reg(platform_read_persistent_reg() & (~BIT_1));
}

void IAP_Params_app_upgrade_flag_set(void){
    platform_write_persistent_reg(platform_read_persistent_reg() | BIT_1);
}

// =================================================================================================
bool IsAppCodeEmpty(void){
    uint8_t *dataPtr = (uint8_t *)APP_START_ADDR;
    for (uint32_t i = 0; i < EFLASH_SECTOR_SIZE; i++) {
        if (dataPtr[i] != 0xFF) {
            return 0; // Not all bytes are 0xFF, return 0
        }
    }
    return 1; // All bytes are 0xFF, return 1
}




// =================================================================================================
void AppJumpToBoot(void){
    IAP_PARAM_DEBUG("app jump to boot.\n");
    IAP_Params_app_upgrade_flag_set();
    platform_switch_app(BOOT_START_ADDR);
}

