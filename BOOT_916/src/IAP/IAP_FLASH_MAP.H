#ifndef _IAP_FLASH_MAP_H_
#define _IAP_FLASH_MAP_H_

// FLASH MAP: https://mauvfmiej7.feishu.cn/docx/MFA4dxejNouFIsxhQAwc3974nXc

#define PLATFORM_START_ADDR         (uint32_t)(0x2002000)
#define BOOT_START_ADDR             (uint32_t)(0x2028000)
#define BOOT_PARAM_ADDR             (uint32_t)(0x202F000)
#define APP_START_ADDR              (uint32_t)(0x2031000)
#define RSVD_START_ADDR             (uint32_t)(0x2040000)
#define APP_PARAM_ADDR              (uint32_t)(0x207E000)
#define FLASH_TOP_ADDR              (uint32_t)(0x2080000)


#define PLATFORM_CODE_SIZE          (BOOT_START_ADDR - PLATFORM_START_ADDR) //152KB
#define BOOT_CODE_SIZE              (BOOT_PARAM_ADDR - BOOT_START_ADDR)     //48KB
#define BOOT_PARAM_SIZE             (APP_START_ADDR  - BOOT_PARAM_ADDR)     //4K*2
#define APP_CODE_SIZE               (RSVD_START_ADDR - APP_START_ADDR)      //64K
#define RSVD_AREA_SIZE              (APP_PARAM_ADDR  - RSVD_START_ADDR)     //224K
#define APP_PARAM_SIZE              (FLASH_TOP_ADDR  - APP_PARAM_ADDR)      //4K*2



#endif

