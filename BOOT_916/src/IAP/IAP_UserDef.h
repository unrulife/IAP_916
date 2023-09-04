#ifndef _IAP_USER_DEF_H_
#define _IAP_USER_DEF_H_

// Erase boot params ???
#define USER_ERASE_BOOT_PARAM_EN        (0)

// Log enable
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

