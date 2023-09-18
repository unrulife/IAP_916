#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include "ingsoc.h"
#include "IAP_UserDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USER_DEF_USB_VID
#define MY_USB_VID          (0x36B0)
#else
#define MY_USB_VID          USER_DEF_USB_VID
#endif

#ifndef USER_DEF_USB_PID
#define MY_USB_PID          (0x0101)
#else
#define MY_USB_PID          USER_DEF_USB_PID
#endif

#ifndef USER_DEF_IAP_REPORT_ID
#define CTL_REPORT_ID       (0x3F)
#else
#define CTL_REPORT_ID       USER_DEF_IAP_REPORT_ID
#endif

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
#define USB_STRING_PRODUCT {16,0x3,'h',0,'i',0,'d',0,'-',0,'i',0,'a',0,'p',0}

typedef struct __attribute__((packed))
{
        uint8_t    size;
        uint8_t    type;
        uint16_t    bcd;
        uint8_t    countryCode;
        uint8_t    nbDescriptor;
        uint8_t    classType0;
        uint16_t    classlength0;
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

#define bNUM_INTERFACES (1)
#define bNUM_EP_IAP (2)

typedef struct __attribute__((packed))
{
    USB_CONFIG_DESCRIPTOR_REAL_T config;
    USB_INTERFACE_DESCRIPTOR_REAL_T interface_iap;
    BSP_USB_HID_DESCRIPTOR_T hid_iap;
    USB_EP_DESCRIPTOR_REAL_T ep_iap[bNUM_EP_IAP];
}BSP_USB_DESC_STRUCTURE_T;

#define SELF_POWERED (1)
#define REMOTE_WAKEUP (0)

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

#define IAP_INTERFACE_IDX   (0x00)

#define USB_INTERFACE_DESCRIPTOR_IAP \
{ \
    .size = sizeof(USB_INTERFACE_DESCRIPTOR_REAL_T), \
    .type = 4, \
    .interfaceIndex = IAP_INTERFACE_IDX, \
    .alternateSetting = 0x00, \
    .nbEp = bNUM_EP_IAP,    \
    .usbClass = 0x03, \
    /* 0: no subclass, 1: boot interface */ \
    .usbSubClass = 0x00, \
    /* 0: none, 1: keyboard, 2: mouse */ \
    .usbProto = 0x00, \
    .iDescription = 0x00 \
}

#define USB_HID_DESCRIPTOR_IAP \
{ \
    .size = sizeof(BSP_USB_HID_DESCRIPTOR_T), \
     /* 0x21: hid descriptor type */ \
    .type = 0x21, \
    .bcd = 0x101, \
    .countryCode = 0x00, \
    .nbDescriptor = 0x01, \
    /* 0x22: report descriptor type */ \
    .classType0 = 0x22, \
    .classlength0 = USB_HID_IAP_REPORT_DESCRIPTOR_SIZE \
}

#define EP_IAP_IN  (1)/* EP1 is in */
#define EP_IAP_OUT (2)
#define EP_X_MPS_BYTES (64)

#define USB_EP_IN_DESCRIPTOR_IAP \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_IN(EP_IAP_IN), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_X_MPS_BYTES, \
    .interval = 0x1 \
}

#define USB_EP_OUT_DESCRIPTOR_IAP \
{ \
    .size = sizeof(USB_EP_DESCRIPTOR_REAL_T), \
    .type = 5, \
    .ep = USB_EP_DIRECTION_OUT(EP_IAP_OUT), \
    .attributes = USB_EP_TYPE_INTERRUPT, \
    .mps = EP_X_MPS_BYTES, \
    .interval = 0x1 \
}

#define MAX_REPORT_SIZE     63   // WITHOUT REPORT ID.

/* 低层每次中断发送的长度为64字节，其中第一字节必须是report id，所以有效字节是后续63字节，所以IN和OUT均设为63.
 */
#define USB_HID_IAP_REPORT_DESCRIPTOR_SIZE (29)
#define USB_HID_IAP_REPORT_DESCRIPTOR {  \
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
    0xC0              /* End Collection */   \
    /* 29 bytes */   \
}

/* 如果OUT的包长定为4095，则PC端应用层每次发送包长均为4096字节（多一字节的reportID），windows底层会自动拆包，按照MPS=64一包一包发送，总共发送64包，
 * 即设备端会回调64次，需要自行组包。设备端是否也可以按照4096去产生中断提示，需要研究usb驱动低层dma接收的方式，有可能行，也有可能不行，目前以项目进度
 * 为准，先不研究这里，后续有时间再研究。下面的报告描述符暂时保留。
 */
// #define USB_HID_IAP_REPORT_DESCRIPTOR_SIZE (30)
// #define USB_HID_IAP_REPORT_DESCRIPTOR {  \
//     0x06, 0x00, 0xFF,  /* Usage Page (Vendor Defined 0xFF00) */   \
//     0x09, 0x01,        /* Usage (0x01) */   \
//     0xA1, 0x01,        /* Collection (Application) */   \
//     0x85, 0x07,        /*   Report ID (7) */   \
//     0x09, 0x01,        /*   Usage (0x01) */   \
//     0x15, 0x00,        /*   Logical Minimum (0) */   \
//     0x26, 0xFF, 0x00,  /*   Logical Maximum (255) */   \
//     0x95, 0x3F,        /*   Report Count (63) */   \
//     0x75, 0x08,        /*   Report Size (8) */   \
//     0x81, 0x02,        /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position) */   \
//     0x09, 0x01,        /*   Usage (0x01) */   \
//     0x96, 0xFF, 0x0F,  /*   Report Count (4095) */   \
//     0x91, 0x02,        /*   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile) */   \
//     0xC0               /* End Collection */   \
//     /* 30 bytes */   \
// }



typedef enum
{
    USB_HID_STA_SUCCESS,            /* > Operate success. */
    USB_HID_STA_NOT_READY,          /* > USB is not ready. */
    USB_HID_STA_BUSY,               /* > Last operating is running. */
    USB_HID_STA_INVALID_PARAM,      /* > Input param error. */
    USB_HID_STA_INACTIVE_EP,        /* > Inactive Endpoint. */
    USB_HID_STA_INTERNAL_ERR,       /* > EP NUM TOO BIG OR INPUT BUFFER NOT ALIGN(4). */
    USB_HID_STA_UNKNOW_ERR,         /* > Unknow error. */
    USB_HID_STA_SIZE_TOO_LARGE,     /* > size too large. */
}USB_HID_IAP_STA_t;


typedef struct
{
    uint8_t pending;
    uint8_t preReady;
    uint8_t ready;
    uint8_t sendBusy;
}BSP_IAP_DATA_s;


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

/**
 * @brief bsp_usb_hid_iap_recv_cb_t
 * @param data: Received data point.
 * @param len:  Received data size. range: [ 1 ~ MAX_REPORT_SIZE ] 
 */
typedef void (* bsp_usb_hid_iap_recv_cb_t)(uint8_t *data, uint16_t len);
typedef void (* bsp_usb_hid_iap_send_complete_cb_t)(void);

extern void bsp_usb_init(void);
extern void bsp_usb_disable(void);
extern void bsp_usb_device_remote_wakeup(void);

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void);
#endif


/**
 * @brief Device sends IAP data to host with this function. 
 *        It will generate a callback if cb is registered with bsp_usb_hid_iap_send_complete_callback_register.
 * 
 * @param data Send data pointer.
 * @param len  Send data size. range: [ 1 ~ MAX_REPORT_SIZE ] 
 * @return USB_HID_IAP_STA_t ref@USB_HID_IAP_STA_t
 */
USB_HID_IAP_STA_t bsp_usb_hid_iap_send(uint8_t *data, uint16_t len);

/**
 * @brief Device sends IAP data to host with bsp_usb_hid_iap_send function.
 *        It will generate a callback if cb is registered by this function.
 * 
 * @param cb 
 */
void bsp_usb_hid_iap_send_complete_callback_register(bsp_usb_hid_iap_send_complete_cb_t cb);

/**
 * @brief If device received data from host, It will generate a callback if
 *        this function is registered.
 * 
 * @param cb application callback function.
 * @param cb param:
 *          data: Received data pointer.
 *          len:  Received data size. range: [ 1 ~ MAX_REPORT_SIZE ] 
 */
void bsp_usb_hid_iap_recv_callback_register(bsp_usb_hid_iap_recv_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif
