#include "IAP_Flash_WP.h"
#include "platform_api.h"
#include "eflash.h"
#include "btstack_util.h"
#include "string.h"

#define FLASH_WP_SIZE       (FLASH_WP_SIZE_384KB)


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
}


void IAP_Flash_lock(void){
    flash_write_protection_set(FLASH_WP_SIZE);
}

void IAP_Flash_Unlock(void){
    flash_write_protection_set(FLASH_WP_SIZE_NONE);
}

void IAP_Flash_WP_Init(void){
    IAP_Flash_lock();
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
