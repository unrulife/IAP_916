#include "IAP_Flash_WP.h"
#include "platform_api.h"
#include "eflash.h"
#include "btstack_util.h"
#include "string.h"
#include "IAP_UserDef.h"

#define BOOT_FLASH_WP_SIZE       (FLASH_WP_SIZE_384KB)


#include "rom_tools.h"

typedef void (*rom_void_void)(void);
typedef void (*rom_FlashSetStatusReg)(uint16_t data);
typedef uint16_t (*rom_FlashGetStatusReg)(void);
#define ROM_FlashWaitBusyDown           ((rom_void_void)(0x00000b6d))
#define ROM_FlashDisableContinuousMode  ((rom_void_void)(0x000007c9))
#define ROM_FlashEnableContinuousMode   ((rom_void_void)(0x0000080d))
#define ROM_DCacheFlush                 ((rom_void_void)(0x00000651))
#define ROM_FlashSetStatusReg  ((rom_FlashSetStatusReg) (0x00000b01))
#define ROM_FlashGetStatusReg  ((rom_FlashGetStatusReg) (0x0000084d))


void flash_read_protection_status(uint8_t *region, uint8_t *reverse_selection){
    ROM_FlashDisableContinuousMode();
    uint16_t status = ROM_FlashGetStatusReg();
    *reverse_selection = ((status>>14)&0x1);
    *region = (0x1F&(status>>2));
    ROM_FlashEnableContinuousMode();
    ROM_FlashWaitBusyDown();
}

void flash_write_protection_set(flash_wp_size_t wp_size){
    switch((uint16_t)wp_size){
        case FLASH_WP_SIZE_NONE:        flash_enable_write_protection(FLASH_REGION_NONE, 0);        break;
        case FLASH_WP_SIZE_4KB:         flash_enable_write_protection(FLASH_REGION_LOWER_1_128, 0); break;
        case FLASH_WP_SIZE_8KB:         flash_enable_write_protection(FLASH_REGION_LOWER_1_64, 0);  break;
        case FLASH_WP_SIZE_16KB:        flash_enable_write_protection(FLASH_REGION_LOWER_1_32, 0);  break;
        case FLASH_WP_SIZE_32KB:        flash_enable_write_protection(FLASH_REGION_LOWER_1_16, 0);  break;
        case FLASH_WP_SIZE_64KB:        flash_enable_write_protection(FLASH_REGION_LOWER_1_8, 0);   break;
        case FLASH_WP_SIZE_128KB:       flash_enable_write_protection(FLASH_REGION_LOWER_1_4, 0);   break;
        case FLASH_WP_SIZE_256KB:       flash_enable_write_protection(FLASH_REGION_LOWER_1_2, 0);   break;
        case FLASH_WP_SIZE_384KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_4, 1);   break;
        case FLASH_WP_SIZE_448KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_8, 1);   break;
        case FLASH_WP_SIZE_480KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_16, 1);  break;
        case FLASH_WP_SIZE_496KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_32, 1);  break;
        case FLASH_WP_SIZE_504KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_64, 1);  break;
        case FLASH_WP_SIZE_508KB:       flash_enable_write_protection(FLASH_REGION_UPPER_1_128, 1); break;
        case FLASH_WP_SIZE_ALL_512KB:   flash_enable_write_protection(FLASH_REGION_ALL, 0);         break;
    }
    ROM_FlashWaitBusyDown();
}

#if USER_FLASH_LOCK_EN
void IAP_Flash_lock(void){
    flash_write_protection_set(BOOT_FLASH_WP_SIZE);
    uint8_t region, reverse;
    flash_read_protection_status(&region, &reverse);
    platform_printf("[WP] region3 = 0x%x, reverse = %d\n", region, reverse);
}
#endif

void IAP_Flash_Unlock(void){
    uint8_t region, reverse;
    flash_read_protection_status(&region, &reverse);
    platform_printf("[WP] region1 = 0x%x, reverse = %d\n", region, reverse);
    flash_write_protection_set(FLASH_WP_SIZE_NONE);
    flash_read_protection_status(&region, &reverse);
    platform_printf("[WP] region2 = 0x%x, reverse = %d\n", region, reverse);
}

void IAP_Flash_WP_Init(void){
    IAP_Flash_Unlock();
}

// =============================================================================================================
#if 0 // for test.

#define FLASH_384KB_ADDR    (0x2000000+(384*1024))
#define FLASH_480KB_ADDR    (0x2000000+(480*1024))
#define FLASH_TEST          (FLASH_480KB_ADDR)

static char wrStr[] = "467r83194072111";

void IAP_Flash_WP_Init(void){
    platform_printf("[WP] test=.\n");

    // flash_write_protection_set(FLASH_WP_SIZE_NONE);
    // flash_write_protection_set(FLASH_WP_SIZE_256KB);
    // flash_write_protection_set(FLASH_WP_SIZE_ALL_512KB);

    // erase.
    uint32_t err = erase_flash_sector(FLASH_TEST);
    if(err){
        platform_printf("[WP] error: erase [%d], ADDR: 0x%08X\n", err, FLASH_TEST);
    }

    // write.
    err = write_flash(FLASH_TEST, (uint8_t *)wrStr, strlen(wrStr));
    if(err){
        platform_printf("[WP] error: write [%d]\n", err);
    }

    // read.
    uint8_t * pData = (uint8_t *)(FLASH_TEST);
    platform_printf("[WP] read: ");
    printf_hexdump(pData, 16);
}

#endif
