#include "IAP_916.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "eflash.h"
#include "rom_tools.h"
#include "string.h"
#include "IAP_UserDef.h"
#include "bsp_usb_hid_kb.h"

// IAP INFO
const char iap_chip[]   = USER_DEF_CHIP_ID;         // max = 15bytes.
const char iap_poject[] = USER_DEF_ITEM_STR;        // max = 23bytes.
const char iap_HW[]     = USER_DEF_HW_VER;          // Vx.y.z [x,y,z = 0~9]
const char iap_SW[]     = USER_DEF_SW_VER;          // Vx.y.z [x,y,z = 0~9]

static IAP_FlashVerInfo_t versionInfo;

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


void AppJumpToBoot(void){
    platform_printf("app jump to boot.\n");
    IAP_Params_app_upgrade_flag_set();
    Uart_Send_Complete_Check();
    for(int i=20000;i>0;i--);
    platform_switch_app(BOOT_START_ADDR);
}

// =================================================================================================
uint16_t getCRC(uint8_t *buffer, uint16_t len){
    return crc(buffer, len);
}

// =================================================================================================
#define IAP_FLASH_VERSION_INFO_ADDR     (BOOT_PARAM_ADDR + EFLASH_ERASABLE_SIZE)


static void IAP_PrintInfo(void){
    platform_printf("\n--------- LOCAL IAP INFOs ----------\n");
    platform_printf("CHIP_INFO : %s\n", iap_chip);
    platform_printf("PROJECT   : %s\n", iap_poject);
    platform_printf("HW        : %s\n", iap_HW);
    platform_printf("SW        : %s\n", iap_SW);
    platform_printf("--------- LOCAL IAP INFOe ----------\n\n");
}

static uint32_t IAP_FlashVersionInfo_ReadToStu(void){
    uint8_t * pData = (uint8_t *)(IAP_FLASH_VERSION_INFO_ADDR);
    memcpy((uint8_t *)&versionInfo, pData, sizeof(IAP_FlashVerInfo_t));
    return IAP_OK;
}

static void PrintString(uint8_t *str, uint16_t len){
    uint16_t i=0;
    for(i=0; i<len; i++){
        platform_printf("%c", str[i]);
    }
    platform_printf("\n");
}

static uint8_t IAP_CheckFlashVersionInfo(void){
    IAP_FlashVersionInfo_ReadToStu();

    if (versionInfo.ver_flag != IAP_VER_FLAG_INITED){
        return 0;
    }

    uint16_t crc16 = getCRC((uint8_t *)&versionInfo.verInfo, sizeof(IAP_AppVerInfoTypedef));
    if(versionInfo.crcVal != crc16){
        platform_printf("version info CRC error, calc[0x%04X], flash[0x%04X].\n", crc16, versionInfo.crcVal);
        return 1;
    }

    platform_printf("\n--------- FLASH IAP INFOs ----------\n");
    platform_printf("CHIP_INFO : ");
    PrintString(versionInfo.verInfo.chip_id.str, versionInfo.verInfo.chip_id.len);
    platform_printf("PROJECT : ");
    PrintString(versionInfo.verInfo.item_info.str, versionInfo.verInfo.item_info.len);
    platform_printf("HW : ");
    PrintString(versionInfo.verInfo.HW, 6);
    platform_printf("SW : ");
    PrintString(versionInfo.verInfo.SW, 6);
    platform_printf("--------- FLASH IAP INFOe ----------\n\n");

    if( versionInfo.verInfo.chip_id.len != strlen(iap_chip) || 
        memcmp(versionInfo.verInfo.chip_id.str, iap_chip, versionInfo.verInfo.chip_id.len) != 0)
    {
        platform_printf("CHIP_INFO different.\n");
        return 2;
    }
    if( versionInfo.verInfo.item_info.len != strlen(iap_poject) || 
        memcmp(versionInfo.verInfo.item_info.str, iap_poject, versionInfo.verInfo.item_info.len) != 0)
    {
        platform_printf("PROJECT different.\n");
        return 3;
    }
    if( 6 != strlen(iap_HW) || 
        memcmp(versionInfo.verInfo.HW, iap_HW, 6) != 0)
    {
        platform_printf("HW different.\n");
        return 4;
    }
    if( 6 != strlen(iap_SW) || 
        memcmp(versionInfo.verInfo.SW, iap_SW, 6) != 0)
    {
        platform_printf("SW different.\n");
        return 5;
    }

    platform_printf("Version OK.\n");

    return 0;
}

static void APP_Task(void *pvParameters){
    static int flag = 10;
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        platform_printf("APP_Task:%d\n", flag);
        if(flag > 0){
            flag--;
        } else {
            if(flag == 0){
                flag = -1;
                platform_printf("go to boot\n");
                bsp_usb_disable();
                vTaskDelay(pdMS_TO_TICKS(500));
                AppJumpToBoot();
            }
        }
    }
}
static void APP_TaskCreate(void){
    xTaskCreate((TaskFunction_t)APP_Task,
                "APP",
                configMINIMAL_STACK_SIZE,
                NULL,
                6,
                NULL);
}

void IAP_Init(void){
    platform_printf("\n===>This is the APP code.\n");
    bsp_usb_init();

    IAP_PrintInfo();
    if(IAP_CheckFlashVersionInfo()){
        platform_printf("go to boot\n");
        bsp_usb_disable();
        AppJumpToBoot();
    }

    APP_TaskCreate();
}

