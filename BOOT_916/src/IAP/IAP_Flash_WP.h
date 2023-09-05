#ifndef _IAP_FLASH_WP_H_
#define _IAP_FLASH_WP_H_

#include <stdint.h>
#include "ingsoc.h"
#include "eflash.h"
#include "rom_tools.h"

#if (INGCHIPS_FAMILY == INGCHIPS_FAMILY_918)
#elif (INGCHIPS_FAMILY == INGCHIPS_FAMILY_916)

// Register offset.
#define FLASH_REG_WP_OFFSET         (2)

// Write protect type.
#define FLASH_WP_TYPE_DIS           (0x00)
#define FLASH_WP_TYPE_EN_ALL        (0x1F)


typedef void (*rom_FlashSetStatusReg)(uint16_t data);
typedef uint16_t (*rom_FlashGetStatusReg)(void);

#define ROM_FlashSetStatusReg  ((rom_FlashSetStatusReg) (0x00000b01))
#define ROM_FlashGetStatusReg  ((rom_FlashGetStatusReg) (0x0000084d))

#endif

void IAP_Flash_WP_Init(void);

#endif
