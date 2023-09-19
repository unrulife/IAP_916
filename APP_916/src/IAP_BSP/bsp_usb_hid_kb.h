#ifndef __BSP_USB_HID_KB_H
#define __BSP_USB_HID_KB_H

#include <stdint.h>
#include "ingsoc.h"
#include "IAP_UserDef.h"

#ifndef USER_DEF_USB_VID
#define MY_USB_VID          (0x36B0)
#else
#define MY_USB_VID          USER_DEF_USB_VID
#endif

#ifndef USER_DEF_USB_PID
#define MY_USB_PID          (0x0102)
#else
#define MY_USB_PID          USER_DEF_USB_PID
#endif

#ifndef USER_DEF_IAP_REPORT_ID
#define CTL_REPORT_ID       (0x2F)
#else
#define CTL_REPORT_ID       USER_DEF_IAP_REPORT_ID
#endif


#define KB_DESCRIPTOR_EN    (1)
#define MO_DESCRIPTOR_EN    (0)
#define CTL_DESCRIPTOR_EN   (1)
#define KB_EXT_DESCRP_EN    (1) // Keyboard with full keys

#ifdef __cplusplus
extern "C" {
#endif

#define U_SUCCESS   (0)
#define U_FAIL      (1)

typedef enum
{
    USB_STA_IDLE,
    USB_STA_BUSY,
} USB_HID_BusySta_t ;

typedef enum
{
    USB_HID_ERROR_NONE,               /* > Operate success. */
    USB_HID_ERROR_NOT_READY,          /* > USB is not ready. */
    USB_HID_ERROR_BUSY,               /* > Last operating is running. */
    USB_HID_ERROR_INVALID_PARAM,      /* > Input param error. */
    USB_HID_ERROR_INACTIVE_EP,        /* > Inactive Endpoint. */
    USB_HID_ERROR_INTERNAL_ERR,       /* > EP NUM TOO BIG OR INPUT BUFFER NOT ALIGN(4). */
    USB_HID_ERROR_UNKNOW_ERR,         /* > Unknow error. */
    USB_HID_ERROR_SIZE_TOO_LARGE,     /* > size too large. */
}USB_HID_OperateSta_t;

// the flag to enable disconnection(cable unplugged), valid only when DP and DM are powered by VBUS
//#define FEATURE_DISCONN_DETECT
// the flag to enable HID function
#define FEATURE_HID_SUPPORT

// ATTENTION ! FIXED IO FOR USB on 916 series
#define USB_PIN_DP GIO_GPIO_16
#define USB_PIN_DM GIO_GPIO_17

#define USB_STRING_LANGUAGE_IDX    0x00
#define USB_STRING_LANGUAGE { 0x04, 0x03, 0x09, 0x04}

#define USB_STRING_MANUFACTURER_IDX    0x01
#define USB_STRING_MANUFACTURER {18,0x3,'I',0,'n',0,'g',0,'c',0,'h',0,'i',0,'p',0,'s',0}

#define USB_STRING_PRODUCT_IDX    0x02
#define USB_STRING_PRODUCT {16,0x3,'h',0,'i',0,'d',0,'-',0,'d',0,'e',0,'v',0}

typedef struct __attribute__((packed))
{
        uint8_t    size;
        uint8_t    type;
        uint16_t   bcd;
        uint8_t    countryCode;
        uint8_t    nbDescriptor;
        uint8_t    classType0;
        uint16_t   classlength0;
} BSP_USB_HID_DESCRIPTOR_T;

typedef enum
{
    USB_REQUEST_HID_CLASS_DESCRIPTOR_HID = 0x21,
    USB_REQUEST_HID_CLASS_DESCRIPTOR_REPORT = 0x22,
    USB_REQUEST_HID_CLASS_DESCRIPTOR_PHYSICAL_DESCRIPTOR = 0x23
} USB_REQUEST_HID_CLASS_DESCRIPTOR_TYPES_E ;

typedef enum
{
    USB_REQUEST_HID_CLASS_REQUEST_REPORT_INPUT = 0x01,
    USB_REQUEST_HID_CLASS_REQUEST_REPORT_OUTPUT = 0x02,
    USB_REQUEST_HID_CLASS_REQUEST_REPORT_FEATURE = 0x03
} USB_REQUEST_HID_CLASS_REQUEST_REPORT_TYPE_E ;

typedef enum
{
    USB_REQUEST_HID_CLASS_REQUEST_GET_REPORT = 0x01,
    USB_REQUEST_HID_CLASS_REQUEST_GET_IDLE = 0x02,
    USB_REQUEST_HID_CLASS_REQUEST_GET_PROTOCOL = 0x03,
    USB_REQUEST_HID_CLASS_REQUEST_SET_REPORT = 0x09,
    USB_REQUEST_HID_CLASS_REQUEST_SET_IDLE = 0x0A,
    USB_REQUEST_HID_CLASS_REQUEST_SET_PROTOCOL = 0x0B
} USB_REQUEST_HID_CLASS_REQUEST_TYPES_E ;

#define USB_DEVICE_DESCRIPTOR \
{ \
        .size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T), \
        .type = 1, \
        .bcdUsb = 0x0200, \
        .usbClass = 0x00, \
        .usbSubClass = 0x00, \
        .usbProto = 0x00, \
        .ep0Mps = USB_EP0_MPS, \
        .vendor = MY_USB_VID, \
        .product = MY_USB_PID, \
        .release = 0x00, \
        .iManufacturer = USB_STRING_MANUFACTURER_IDX, \
        .iProduct = USB_STRING_PRODUCT_IDX, \
        .iSerial = 0x00, \
        .nbConfig = 0x01 \
}

#if KB_DESCRIPTOR_EN
#define KB_INTERFACES_NUM   (1)
#else
#define KB_INTERFACES_NUM   (0)
#endif

#if MO_DESCRIPTOR_EN
#define MO_INTERFACES_NUM   (1)
#else
#define MO_INTERFACES_NUM   (0)
#endif

#if CTL_DESCRIPTOR_EN
#define CTL_INTERFACES_NUM  (1)
#else
#define CTL_INTERFACES_NUM  (0)
#endif

#define bNUM_INTERFACES (KB_INTERFACES_NUM + MO_INTERFACES_NUM + CTL_INTERFACES_NUM)

#if KB_DESCRIPTOR_EN
#define KB_INTERFACE_IDX    (0x00)
#define bNUM_EP_KB          (1)
#define EP_KB_IN            (1)/* EP1 is in */
#define EP_KB_IDX_GET(x)    (x - EP_KB_IN) // ep_kb[x]
#define EP_KB_MPS_BYTES     (8)
#endif

#if MO_DESCRIPTOR_EN
#define MO_INTERFACE_IDX    (0x01)
#define bNUM_EP_MO          (1)
#define EP_MO_IN            (2)/* EP2 is in */
#define EP_MO_IDX_GET(x)    (x - EP_MO_IN) // ep_mo[x]
#define EP_MO_MPS_BYTES     (8)
#endif

#if CTL_DESCRIPTOR_EN
#define CTL_INTERFACE_IDX   (0x02)
#define bNUM_EP_CTL         (2)
#define EP_CTL_IN           (3)/* EP3 is in */
#define EP_CTL_OUT          (4)/* EP4 is out */
#define EP_CTL_IDX_GET(x)   (x - EP_CTL_IN) // ep_ctl[x]
#define EP_CTL_MPS_BYTES    (64)
#endif

#define SELF_POWERED        (1)
#define REMOTE_WAKEUP       (0)

// =======================================================================================
typedef struct __attribute__((packed))
{
    /* config */
    USB_CONFIG_DESCRIPTOR_REAL_T config;

#if KB_DESCRIPTOR_EN
    /* keyboard */
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_kb;
    BSP_USB_HID_DESCRIPTOR_T hid_kb;
    USB_EP_DESCRIPTOR_REAL_T ep_kb[bNUM_EP_KB];
#endif

#if MO_DESCRIPTOR_EN
    /* mouse */
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_mo;
    BSP_USB_HID_DESCRIPTOR_T hid_mo;
    USB_EP_DESCRIPTOR_REAL_T ep_mo[bNUM_EP_MO];
#endif

#if CTL_DESCRIPTOR_EN
    /* control */
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_ctl;
    BSP_USB_HID_DESCRIPTOR_T hid_ctl;
    USB_EP_DESCRIPTOR_REAL_T ep_ctl[bNUM_EP_CTL];
#endif

}BSP_USB_DESC_STRUCTURE_T;

// =======================================================================================
#define USB_CONFIG_DESCRIPTOR \
{ \
    .size = sizeof(USB_CONFIG_DESCRIPTOR_REAL_T), \
    .type = 2, \
    .totalLength = sizeof(BSP_USB_DESC_STRUCTURE_T), \
    .nbInterface = bNUM_INTERFACES, \
    .configIndex = 0x01, \
    .iDescription = 0x00, \
    .attrib = 0x80| (SELF_POWERED<<6) | (REMOTE_WAKEUP<<5), \
    .maxPower = 0xFA \
}

// =======================================================================================
#if KB_DESCRIPTOR_EN
#define USB_INTERFACE_DESCRIPTOR_KB \
{ \
    .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
    .type = 4, \
    .interfaceIndex = KB_INTERFACE_IDX, \
    .alternateSetting = 0x00, \
    .nbEp = bNUM_EP_KB,    \
    .usbClass = 0x03, \
    /* 0: no subclass, 1: boot interface */ \
    .usbSubClass = 0x00, \
    /* 0: none, 1: keyboard, 2: mouse */ \
    .usbProto = 0x00, \
    .iDescription = 0x00 \
}

#define USB_HID_DESCRIPTOR_KB \
{ \
    .size = sizeof(BSP_USB_HID_DESCRIPTOR_T), \
     /* 0x21: hid descriptor type */ \
    .type = 0x21, \
    .bcd = 0x101, \
    .countryCode = 0x00, \
    .nbDescriptor = 0x01, \
    /* 0x22: report descriptor type */ \
    .classType0 = 0x22, \
    .classlength0 = USB_HID_KB_REPORT_DESCRIPTOR_SIZE \
}


#define USB_EP_IN_DESCRIPTOR_KB \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_IN(EP_KB_IN), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_KB_MPS_BYTES, \
    .interval = 0x1 \
}

#define USB_HID_KB_REPORT_DESCRIPTOR_SIZE (63)
#define USB_HID_KB_REPORT_DESCRIPTOR {    \
        0x05, 0x01, /* USAGE_PAGE (Generic Desktop)                                             */    \
        0x09, 0x06, /* USAGE (Keyboard)                                                                     */    \
        0xa1, 0x01, /* COLLECTION (Application)                                                     */    \
        0x05, 0x07, /*     USAGE_PAGE (Keyboard)                                                        */    \
        0x19, 0xe0, /*     USAGE_MINIMUM (Keyboard LeftControl)                         */    \
        0x29, 0xe7, /*     USAGE_MAXIMUM (Keyboard Right GUI)                             */    \
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)                                                            */    \
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)                                                            */    \
        0x75, 0x01, /*     REPORT_SIZE (1)                                                                    */    \
        0x95, 0x08, /*     REPORT_COUNT (8)                                                                 */    \
        0x81, 0x02, /*     INPUT (Data,Var,Abs)                                                         */    \
        0x95, 0x01, /*     REPORT_COUNT (1)                                                                 */    \
        0x75, 0x08, /*     REPORT_SIZE (8)                                                                    */    \
        0x81, 0x03, /*     INPUT (Cnst,Var,Abs)                                                         */    \
        0x95, 0x05, /*     REPORT_COUNT (5)                                                                 */    \
        0x75, 0x01, /*     REPORT_SIZE (1)                                                                    */    \
        0x05, 0x08, /*     USAGE_PAGE (LEDs)                                                                */    \
        0x19, 0x01, /*     USAGE_MINIMUM (Num Lock)                                                 */    \
        0x29, 0x05, /*     USAGE_MAXIMUM (Kana)                                                         */    \
        0x91, 0x02, /*     OUTPUT (Data,Var,Abs)                                                        */    \
        0x95, 0x01, /*     REPORT_COUNT (1)                                                                 */    \
        0x75, 0x03, /*     REPORT_SIZE (3)                                                                    */    \
        0x91, 0x03, /*     OUTPUT (Cnst,Var,Abs)                                                        */    \
        0x95, 0x06, /*     REPORT_COUNT (6)                                                                 */    \
        0x75, 0x08, /*     REPORT_SIZE (8)                                                                    */    \
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)                                                            */    \
        0x25, 0x65, /*     LOGICAL_MAXIMUM (101)                                                        */    \
        0x05, 0x07, /*     USAGE_PAGE (Keyboard)                                                        */    \
        0x19, 0x00, /*     USAGE_MINIMUM (Reserved (no event indicated))        */    \
        0x29, 0x65, /*     USAGE_MAXIMUM (Keyboard Application)                         */    \
        0x81, 0x00, /*     INPUT (Data,Ary,Abs)                                                         */    \
        0xc0                /* END_COLLECTION                                                                         */    \
}
#endif // #if KB_DESCRIPTOR_EN

// =======================================================================================
#if MO_DESCRIPTOR_EN
#define USB_INTERFACE_DESCRIPTOR_MO \
{ \
    .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
    .type = 4, \
    .interfaceIndex = MO_INTERFACE_IDX, \
    .alternateSetting = 0x00, \
    .nbEp = bNUM_EP_MO,    \
    .usbClass = 0x03, \
    /* 0: no subclass, 1: boot interface */ \
    .usbSubClass = 0x00, \
    /* 0: none, 1: keyboard, 2: mouse */ \
    .usbProto = 0x00, \
    .iDescription = 0x00 \
}

#define USB_HID_DESCRIPTOR_MO \
{ \
    .size = sizeof(BSP_USB_HID_DESCRIPTOR_T), \
     /* 0x21: hid descriptor type */ \
    .type = 0x21, \
    .bcd = 0x101, \
    .countryCode = 0x00, \
    .nbDescriptor = 0x01, \
    /* 0x22: report descriptor type */ \
    .classType0 = 0x22, \
    .classlength0 = USB_HID_MOUSE_REPORT_DESCRIPTOR_SIZE \
}


#define USB_EP_IN_DESCRIPTOR_MO \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_IN(EP_MO_IN), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_MO_MPS_BYTES, \
    .interval = 0xA \
}

#define USB_HID_MOUSE_REPORT_DESCRIPTOR_SIZE (50)
#define USB_HID_MOUSE_REPORT_DESCRIPTOR { \
        0x05, 0x01, /* USAGE_PAGE (Generic Desktop)             */     \
        0x09, 0x02, /* USAGE (Mouse)                                            */     \
        0xa1, 0x01, /* COLLECTION (Application)                     */     \
        0x09, 0x01, /*     USAGE (Pointer)                                    */     \
        0xa1, 0x00, /*     COLLECTION (Physical)                        */     \
        0x05, 0x09, /*         USAGE_PAGE (Button)                        */     \
        0x19, 0x01, /*         USAGE_MINIMUM (Button 1)             */     \
        0x29, 0x03, /*         USAGE_MAXIMUM (Button 3)             */     \
        0x15, 0x00, /*         LOGICAL_MINIMUM (0)                        */     \
        0x25, 0x01, /*         LOGICAL_MAXIMUM (1)                        */     \
        0x95, 0x03, /*         REPORT_COUNT (3)                             */     \
        0x75, 0x01, /*         REPORT_SIZE (1)                                */     \
        0x81, 0x02, /*         INPUT (Data,Var,Abs)                     */     \
        0x95, 0x01, /*         REPORT_COUNT (1)                             */     \
        0x75, 0x05, /*         REPORT_SIZE (5)                                */     \
        0x81, 0x03, /*         INPUT (Cnst,Var,Abs)                     */     \
        0x05, 0x01, /*         USAGE_PAGE (Generic Desktop)     */     \
        0x09, 0x30, /*         USAGE (X)                                            */     \
        0x09, 0x31, /*         USAGE (Y)                                            */     \
        0x15, 0x81, /*         LOGICAL_MINIMUM (-127)                 */     \
        0x25, 0x7f, /*         LOGICAL_MAXIMUM (127)                    */     \
        0x75, 0x08, /*         REPORT_SIZE (8)                                */     \
        0x95, 0x02, /*         REPORT_COUNT (2)                             */     \
        0x81, 0x06, /*         INPUT (Data,Var,Rel)                     */     \
        0xc0,             /*     END_COLLECTION                                     */     \
        0xc0                /* END_COLLECTION                                         */     \
}
#endif // #if MO_DESCRIPTOR_EN

// =======================================================================================
#if CTL_DESCRIPTOR_EN

#define USB_INTERFACE_DESCRIPTOR_CTL \
{ \
    .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
    .type = 4, \
    .interfaceIndex = CTL_INTERFACE_IDX, \
    .alternateSetting = 0x00, \
    .nbEp = bNUM_EP_CTL,    \
    .usbClass = 0x03, \
    /* 0: no subclass, 1: boot interface */ \
    .usbSubClass = 0x00, \
    /* 0: none, 1: keyboard, 2: mouse */ \
    .usbProto = 0x00, \
    .iDescription = 0x00 \
}

#define USB_HID_DESCRIPTOR_CTL \
{ \
    .size = sizeof(BSP_USB_HID_DESCRIPTOR_T), \
     /* 0x21: hid descriptor type */ \
    .type = 0x21, \
    .bcd = 0x101, \
    .countryCode = 0x00, \
    .nbDescriptor = 0x01, \
    /* 0x22: report descriptor type */ \
    .classType0 = 0x22, \
    .classlength0 = USB_HID_CTL_REPORT_DESCRIPTOR_SIZE \
}

#define USB_EP_IN_DESCRIPTOR_CTL \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_IN(EP_CTL_IN), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_CTL_MPS_BYTES, \
    .interval = 0x1 \
}

#define USB_EP_OUT_DESCRIPTOR_CTL \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_OUT(EP_CTL_OUT), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_CTL_MPS_BYTES, \
    .interval = 0x1 \
}

#define MAX_REPORT_SIZE     63   // WITHOUT REPORT ID.

#define KB_REPORT_ID        0x06 

/* 底层每次中断发送的长度为64字节，其中第一字节必须是report id，所以有效字节是后续63字节，所以IN和OUT均设为63.
 */
#define USB_HID_CTL_REPORT_DESCRIPTOR_SIZE (29+25)
#define USB_HID_CTL_REPORT_DESCRIPTOR {  \
    0x06, 0x00, 0xFF,  /* Usage Page (Vendor Defined 0xFF00) */   \
    0x09, 0x01,        /* Usage (0x01) */   \
    0xA1, 0x01,        /* Collection (Application) */   \
    0x85, CTL_REPORT_ID,   /*   Report ID (7) */   \
    0x09, 0x01,        /*   Usage (0x01) */   \
    0x15, 0x00,        /*   Logical Minimum (0) */   \
    0x26, 0xFF, 0x00,  /*   Logical Maximum (255) */   \
    0x95, MAX_REPORT_SIZE,        /*   Report Count (63) */   \
    0x75, 0x08,        /*   Report Size (8) */   \
    0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */   \
    0x09, 0x01,        /*   Usage (0x01) */   \
    0x95, MAX_REPORT_SIZE,        /*   Report Count (63) */   \
    0x91, 0x02,        /*   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */   \
    0xC0,              /* End Collection */   \
    /* 29 bytes */   \
    0x05, 0x01,        /* GLOBAL_USAGE_PAGE(Generic Desktop Controls)     */  \
    0x09, 0x06,        /* LOCAL_USAGE(Keyboard)     */  \
    0xA1, 0x01,        /* MAIN_COLLECTION(Application)     */  \
    0x85, KB_REPORT_ID,/* GLOBAL_REPORT_ID     */  \
    0x05, 0x07,        /* GLOBAL_USAGE_PAGE(Keyboard/Keypad)     */  \
    0x19, 0x04,        /* LOCAL_USAGE_MINIMUM(4)     */  \
    0x29, 0x70,        /* LOCAL_USAGE_MAXIMUM(112)     */  \
    0x15, 0x00,        /* GLOBAL_LOGICAL_MINIMUM(0)     */  \
    0x25, 0x01,        /* GLOBAL_LOCAL_MAXIMUM(1)     */  \
    0x75, 0x01,        /* GLOBAL_REPORT_SIZE(1)     */  \
    0x95, 0x78,        /* GLOBAL_REPORT_COUNT(120)     */  \
    0x81, 0x02,        /* MAIN_INPUT(data var absolute NoWrap linear PreferredState NoNullPosition NonVolatile )    Input 18.0 */  \
    0xC0               /* MAIN_COLLECTION_END */  \
    /* 25 bytes */   \
}

#endif

// =======================================================================================
#if KB_DESCRIPTOR_EN
typedef enum
{
    HID_KEYB_A = 4,
    HID_KEYB_B = 5,
    HID_KEYB_C = 6,
    HID_KEYB_D = 7,
    HID_KEYB_E = 8,
    HID_KEYB_F = 9,
    HID_KEYB_G = 10,
    HID_KEYB_H = 11,
    HID_KEYB_I = 12,
    HID_KEYB_J = 13,
    HID_KEYB_K = 14,
    HID_KEYB_L = 15,
    HID_KEYB_M = 16,
    HID_KEYB_N = 17,
    HID_KEYB_O = 18,
    HID_KEYB_P = 19,
    HID_KEYB_Q = 20,
    HID_KEYB_R = 21,
    HID_KEYB_S = 22,
    HID_KEYB_T = 23,
    HID_KEYB_U = 24,
    HID_KEYB_V = 25,
    HID_KEYB_W = 26,
    HID_KEYB_X = 27,
    HID_KEYB_Y = 28,
    HID_KEYB_Z = 29,
    HID_KEYB_1 = 30,
    HID_KEYB_2 = 31,
    HID_KEYB_3 = 32,
    HID_KEYB_4 = 33,
    HID_KEYB_5 = 34,
    HID_KEYB_6 = 35,
    HID_KEYB_7 = 36,
    HID_KEYB_8 = 37,
    HID_KEYB_9 = 38,
    HID_KEYB_0 = 39,
    HID_KEYB_ENTER = 40,
    HID_KEYB_ESCAPE = 41,
    HID_KEYB_BACKSPACE = 42,
    HID_KEYB_TAB = 43,
    HID_KEYB_SPACEBAR = 44,
    HID_KEYB_UNDERSCORE = 45,
    HID_KEYB_PLUS = 46,
    HID_KEYB_OPEN_BRACKET = 47,
    HID_KEYB_CLOSE_BRACKET = 48,
    HID_KEYB_BACKSLASH = 49,
    HID_KEYB_ASH = 50,
    HID_KEYB_COLON = 51,
    HID_KEYB_QUOTE = 52,
    HID_KEYB_TILDE = 53,
    HID_KEYB_COMMA = 54,
    HID_KEYB_DOT = 55,
    HID_KEYB_SLASH = 56,
    HID_KEYB_CAPS_LOCK = 57,
    HID_KEYB_F1 = 58,
    HID_KEYB_F2 = 59,
    HID_KEYB_F3 = 60,
    HID_KEYB_F4 = 61,
    HID_KEYB_F5 = 62,
    HID_KEYB_F6 = 63,
    HID_KEYB_F7 = 64,
    HID_KEYB_F8 = 65,
    HID_KEYB_F9 = 66,
    HID_KEYB_F10 = 67,
    HID_KEYB_F11 = 68,
    HID_KEYB_F12 = 69,
    HID_KEYB_PRINTSCREEN = 70,
    HID_KEYB_SCROLL_LOCK = 71,
    HID_KEYB_PAUSE = 72,
    HID_KEYB_INSERT = 73,
    HID_KEYB_HOME = 74,
    HID_KEYB_PAGEUP = 75,
    HID_KEYB_DELETE = 76,
    HID_KEYB_END = 77,
    HID_KEYB_PAGEDOWN = 78,
    HID_KEYB_RIGHT = 79,
    HID_KEYB_LEFT = 80,
    HID_KEYB_DOWN = 81,
    HID_KEYB_UP = 82,
    HID_KEYB_KEYPAD_NUM_LOCK = 83,
    HID_KEYB_KEYPAD_DIVIDE = 84,
    HID_KEYB_KEYPAD_AT = 85,
    HID_KEYB_KEYPAD_MULTIPLY = 85,
    HID_KEYB_KEYPAD_MINUS = 86,
    HID_KEYB_KEYPAD_PLUS = 87,
    HID_KEYB_KEYPAD_ENTER = 88,
    HID_KEYB_KEYPAD_1 = 89,
    HID_KEYB_KEYPAD_2 = 90,
    HID_KEYB_KEYPAD_3 = 91,
    HID_KEYB_KEYPAD_4 = 92,
    HID_KEYB_KEYPAD_5 = 93,
    HID_KEYB_KEYPAD_6 = 94,
    HID_KEYB_KEYPAD_7 = 95,
    HID_KEYB_KEYPAD_8 = 96,
    HID_KEYB_KEYPAD_9 = 97,
    HID_KEYB_KEYPAD_0 = 98,
    HID_KEYB_KEYPAD_DOT = 99
} BSP_KEYB_KEYB_USAGE_ID_e;

typedef enum
{
    HID_KEYB_MODIFIER_LEFT_CTRL = 0x01,
    HID_KEYB_MODIFIER_LEFT_SHIFT = 0x02,
    HID_KEYB_MODIFIER_LEFT_ALT = 0x04,
    HID_KEYB_MODIFIER_LEFT_UI = 0x08,
    HID_KEYB_MODIFIER_RIGHT_CTRL = 0x10,
    HID_KEYB_MODIFIER_RIGHT_SHIFT = 0x20,
    HID_KEYB_MODIFIER_RIGHT_ALT = 0x40,
    HID_KEYB_MODIFIER_RIGHT_UI = 0x80
} BSP_KEYB_KEYB_MODIFIER_e;

typedef enum
{
    HID_KEYB_LED_NUM_LOCK = 0x01,
    HID_KEYB_LED_CAPS_LOCK = 0x02,
    HID_KEYB_LED_SCROLL_LOCK = 0x04,
    HID_KEYB_LED_COMPOSE = 0x08,
    HID_KEYB_LED_KANA = 0x10
} BSP_KEYB_KEYB_LED_e;

typedef enum {
    KEY_TYPE_GENERAL,
    KEY_TYPE_MODIFIER,
} BSP_HID_KB_Type_t;

#define KEY_TABLE_LEN (6)
typedef struct __attribute__((packed))
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t key_table[KEY_TABLE_LEN];
}BSP_KEYB_REPORT_s;

#define EXT_KEY_TABLE_LEN (15)

typedef struct
{
    BSP_KEYB_REPORT_s report;
#if KB_EXT_DESCRP_EN
    uint8_t ext_key_table[EXT_KEY_TABLE_LEN];
    uint8_t extend_flag;
    uint8_t basic_send_flag;
    uint8_t extend_send_flag;
#endif
    uint8_t led_state;
    uint8_t sendBusy;
}BSP_KEYB_DATA_s;
#endif

// =======================================================================================
#if MO_DESCRIPTOR_EN
#define USB_HID_MO_BUTTON_POS     0
#define USB_HID_MO_X_POS         1
#define USB_HID_MO_Y_POS         2
#define USB_HID_MO_MAX_POS         3

typedef struct __attribute__((packed))
{
    uint8_t button;/* 1 ~ 3 */
    int8_t pos_x;/* -127 ~ 127 */
    int8_t pos_y;/* -127 ~ 127 */
}BSP_MOUSE_REPORT_s;

typedef struct
{
    BSP_MOUSE_REPORT_s report;
    uint8_t pending;
}BSP_MOUSE_DATA_s;
#endif

// =======================================================================================
#if CTL_DESCRIPTOR_EN
typedef struct
{
    uint8_t preReady;
    uint8_t ready;
    uint8_t sendBusy;
}BSP_CTL_DATA_s;

/**
 * @brief bsp_usb_hid_ctl_recv_cb_t
 * @param data: Received data point.
 * @param len:  Received data size. range: [ 1 ~ MAX_REPORT_SIZE ] 
 */
typedef void (* bsp_usb_hid_ctl_recv_cb_t)(uint8_t *data, uint16_t len);
typedef void (* bsp_usb_hid_ctl_send_complete_cb_t)(void);
#endif


// =======================================================================================
#if KB_DESCRIPTOR_EN
/**
 * @brief interface API. send keyboard key report
 *
 * @param[in] key: value comes from BSP_KEYB_KEYB_USAGE_ID_e
 * @param[in] press: 1: pressed, 0: released
 * @param[out] null. 
 */
// extern void bsp_usb_handle_hid_keyb_key_report(uint8_t key, uint8_t press);
/**
 * @brief interface API. send keyboard modifier report
 *
 * @param[in] modifier: value comes from BSP_KEYB_KEYB_MODIFIER_e
 * @param[in] press: 1: pressed, 0: released
 * @param[out] null. 
 */
// extern void bsp_usb_handle_hid_keyb_modifier_report(BSP_KEYB_KEYB_MODIFIER_e modifier, uint8_t press);
/**
 * @brief interface API. get keyboard led state
 *
 * @param[in] null
 * @param[out] 8bit value, refer to BSP_KEYB_KEYB_LED_e. 
 */
extern uint8_t bsp_usb_get_hid_keyb_led_report(void);
#endif
// =======================================================================================
#if MO_DESCRIPTOR_EN
/**
 * @brief interface API. send mouse report
 *
 * @param[in] x: 8bit int x axis value, relative,
 * @param[in] y: 8bit int y axis value, relative,
 * @param[in] btn: 8bit value, button 1 to 3,
 * @param[out] null. 
 */
extern void bsp_usb_handle_hid_mouse_report(int8_t x, int8_t y, uint8_t btn);
#endif
// =======================================================================================
#if CTL_DESCRIPTOR_EN

extern USB_HID_OperateSta_t bsp_usb_hid_ctl_send(uint8_t *data, uint16_t len);
extern void bsp_usb_hid_ctl_send_complete_callback_register(bsp_usb_hid_ctl_send_complete_cb_t cb);
extern void bsp_usb_hid_ctl_recv_callback_register(bsp_usb_hid_ctl_recv_cb_t cb);

#if KB_EXT_DESCRP_EN
/**
 * @brief Pre-set extend key value, but don't send it to usb endpoint. Use bsp_usb_hid_keyboard_extend_report_start
 * to trigger sending. You can call this function multiple times to load multiple keys, and you only need to call 
 * the bsp_usb_hid_keyboard_extend_report_start function once to trigger the sending process of all the loaded keys. 
 * 
 * @param key  ref@BSP_KEYB_KEYB_USAGE_ID_e
 * @param press 1:pressed, 0:released.
 */
extern uint8_t bsp_usb_hid_keyboard_extend_report_set_key_value(uint8_t key, uint8_t press);
/**
 * @brief Send pre-set extend key value to usb endpoint. ref: bsp_usb_hid_keyboard_extend_report_set_key_value
 * Note that if calling this function returns USB_HID_ERROR_BUSY, you will have to call it again at another time to ensure 
 * that the key value was successfully sent. 
 * 
 * @return USB_HID_OperateSta_t 
 */
extern USB_HID_OperateSta_t bsp_usb_hid_keyboard_extend_report_start(void);

#define USE_SOF_TRIGGER_KB_SEND_EN      (1)

#if USE_SOF_TRIGGER_KB_SEND_EN
#else
typedef void (* bsp_usb_hid_kb_basic_delay_send_cb_t)(void);
typedef void (* bsp_usb_hid_kb_extend_delay_send_cb_t)(void);
void bsp_usb_hid_kb_basic_delay_send_callback_register(bsp_usb_hid_kb_basic_delay_send_cb_t cb);
extern void bsp_usb_hid_kb_extend_delay_send_callback_register(bsp_usb_hid_kb_extend_delay_send_cb_t cb);
#endif
extern void bsp_usb_hid_kb_key_report(BSP_HID_KB_Type_t type, uint8_t key, uint8_t press);
#endif

#endif


// =======================================================================================

typedef enum
{
    BSP_USB_PHY_DISABLE,
    BSP_USB_PHY_ENABLE
}BSP_USB_PHY_ENABLE_e;

typedef enum
{
    BSP_USB_PHY_DP_PULL_UP = 1,
    BSP_USB_PHY_DM_PULL_UP,
    BSP_USB_PHY_DP_DM_PULL_DOWN
}BSP_USB_PHY_PULL_e;

typedef struct
{
    uint32_t remote_wakeup:1;
    uint32_t unused:31;
}BSP_USB_VAR_s;


extern void bsp_usb_init(void);
extern void bsp_usb_disable(void);
extern void bsp_usb_device_remote_wakeup(void);

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void);
#endif


#ifdef __cplusplus
}
#endif

#endif
