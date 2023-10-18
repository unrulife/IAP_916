#ifndef _IAP_USER_DEF_H_
#define _IAP_USER_DEF_H_

// Log enable/disable
#define USER_IAP_MAIN_DEBUG_LOG_EN      (1)
#define USER_IAP_MAIN_ERROR_LOG_EN      (1)
#define USER_IAP_APP_DEBUG_LOG_EN       (0)
#define USER_IAP_APP_ERROR_LOG_EN       (1)
#define USER_IAP_TRANSPORT_DEBUG_EN     (0)
#define USER_IAP_TRANSPORT_ERROR_EN     (1)

// version information.
#define USER_DEF_CHIP_ID                "ING91683C_TB"          // max = 15bytes.
#define USER_DEF_ITEM_STR               "INGCHIPS_IAP_TEST_APP" // max = 23bytes.
#define USER_DEF_HW_VER                 "V1.0.4"                // Vx.y.z [x,y,z = 0~9]
#define USER_DEF_SW_VER                 "V2.2.8"                // Vx.y.z [x,y,z = 0~9]

// app usb information.
#define USER_DEF_USB_VID                (0x36B0)
#define USER_DEF_USB_PID                (0x0102)
#define USER_DEF_IAP_REPORT_ID          (0x2F)

#endif

