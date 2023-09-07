#ifndef _IAP_USER_DEF_H_
#define _IAP_USER_DEF_H_

// Product config.
#define USER_FLASH_LOCK_EN              (0)     /* <= Enable flash protection function. The test phase can disable, 
                                                        and the MP phase is strongly recommended to enable. */

// Test config.
#define USER_ERASE_BOOT_PARAM_EN        (0)     /* <= For test. Enable erase boot params area for startup. */
#define USER_APP_CODE_CHECK_CRC_EN      (0)     /* <= For test. Enable checking CRC of APP before jumping to APP. */

// Log enable/disable
#define USER_IAP_MAIN_DEBUG_LOG_EN      (1)
#define USER_IAP_MAIN_ERROR_LOG_EN      (1)
#define USER_IAP_PARAM_DEBUG_LOG_EN     (0)
#define USER_IAP_PARAM_ERROR_LOG_EN     (1)
#define USER_IAP_APP_DEBUG_LOG_EN       (0)
#define USER_IAP_APP_ERROR_LOG_EN       (1)
#define USER_IAP_TRANSPORT_DEBUG_EN     (0)
#define USER_IAP_TRANSPORT_ERROR_EN     (1)

// version information.
#define USER_DEF_CHIP_ID                "ING91683C_TB"          // max = 15bytes.
#define USER_DEF_ITEM_STR               "INGCHIPS_IAP_TEST_APP" // max = 23bytes.


#endif

