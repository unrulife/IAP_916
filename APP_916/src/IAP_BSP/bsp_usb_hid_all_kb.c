#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb_hid_all_kb.h"
#include "btstack_util.h"

#if 0
#define USB_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define USB_DEBUG(...)      
#endif

#if 1
#define USB_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define USB_ERROR(...)      
#endif

// =============================================================================================================

const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;
const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) =
{
    USB_CONFIG_DESCRIPTOR
#if KB_DESCRIPTOR_EN
    ,USB_INTERFACE_DESCRIPTOR_KB,  USB_HID_DESCRIPTOR_KB,  {USB_EP_IN_DESCRIPTOR_KB}
#endif
#if MO_DESCRIPTOR_EN
    ,USB_INTERFACE_DESCRIPTOR_MO,  USB_HID_DESCRIPTOR_MO,  {USB_EP_IN_DESCRIPTOR_MO}
#endif
#if CTL_DESCRIPTOR_EN
    ,USB_INTERFACE_DESCRIPTOR_CTL, USB_HID_DESCRIPTOR_CTL, {USB_EP_IN_DESCRIPTOR_CTL, USB_EP_OUT_DESCRIPTOR_CTL}
#endif
};
const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;
uint8_t DynamicDescriptor[64] __attribute__ ((aligned (4)));

BSP_USB_VAR_s UsbVar;

// =============================================================================================================
#if KB_DESCRIPTOR_EN
const uint8_t ReportKeybDescriptor[] __attribute__ ((aligned (4))) = USB_HID_KB_REPORT_DESCRIPTOR;
BSP_KEYB_DATA_s KeybReport __attribute__ ((aligned (4))) = {.sendBusy = U_FALSE};
#endif

#if MO_DESCRIPTOR_EN
const uint8_t ReportMouseDescriptor[] __attribute__ ((aligned (4))) = USB_HID_MOUSE_REPORT_DESCRIPTOR;
BSP_MOUSE_DATA_s MouseReport __attribute__ ((aligned (4))) = {.pending = U_TRUE};
#endif

#if CTL_DESCRIPTOR_EN
uint8_t DataRecvBuf[EP_CTL_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataSendBuf[EP_CTL_MPS_BYTES] __attribute__ ((aligned (4)));
const uint8_t ReportCtlDescriptor[] __attribute__ ((aligned (4))) = USB_HID_CTL_REPORT_DESCRIPTOR;
BSP_CTL_DATA_s CtlReport __attribute__ ((aligned (4))) = {
    .preReady = U_FALSE,
    .ready = U_FALSE,
    .sendBusy = U_FALSE
};

static bsp_usb_hid_ctl_recv_cb_t              usb_hid_ctl_recv_callback = NULL;
static bsp_usb_hid_ctl_send_complete_cb_t     usb_hid_ctl_send_complete_callback = NULL;

static void bsp_usb_hid_ctl_push_rx_data_to_user(uint8_t *data, uint16_t len);
static void bsp_usb_hid_ctl_push_send_complete_to_user(void);
static USB_ERROR_TYPE_E bsp_usp_hid_ctl_rx_data_trigger(uint8_t printFLAG);
static USB_ERROR_TYPE_E bsp_usp_hid_ctl_tx_data_trigger(uint8_t reportID, uint8_t *data, uint16_t len);
static void usb_reset_keyboard_init(void);
#if USE_SOF_TRIGGER_KB_SEND_EN
static void usb_sof_keyboard_send_check(void);
#endif
#endif

// =============================================================================================================
static uint32_t bsp_usb_event_handler(USB_EVNET_HANDLER_T *event)
{
    uint32_t size;
    uint32_t status = USB_ERROR_NONE;

    USB_DEBUG("\n\n------------- evt_id:%d ------------\n", event->id);

    switch(event->id)
    {
        case USB_EVENT_DEVICE_RESET:
        {
            usb_reset_keyboard_init();
            #ifdef FEATURE_DISCONN_DETECT
            platform_set_timer(bsp_usb_device_disconn_timeout,160);
            #endif
            USB_DEBUG("#USB RESET\n");
        }break;
        case USB_EVENT_DEVICE_SOF:
        {
#if USE_SOF_TRIGGER_KB_SEND_EN
            usb_sof_keyboard_send_check();
#endif
            // USB_DEBUG("#USB SOF\n");
            // handle sof, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_DEVICE_SUSPEND:
        {
            USB_DEBUG("#USB SUSPEND\n");
#if CTL_DESCRIPTOR_EN
            CtlReport.ready = U_FALSE;
#endif
            // handle suspend, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_DEVICE_RESUME:
        {
            USB_DEBUG("#USB RESUME\n");
#if CTL_DESCRIPTOR_EN
            CtlReport.ready = U_TRUE;
#endif
            // handle resume, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_EP0_SETUP:
        {

            USB_SETUP_T* setup = USB_GetEp0SetupData();

            if (setup->bmRequestType.Direction){
                USB_DEBUG("#USB EP0 SETUP: Recipient(%d), Type(%d), Direction(S->M), bRequest(%d) \n",setup->bmRequestType.Recipient,
                                                                                                    setup->bmRequestType.Type,
                                                                                                    setup->bRequest
                                                                                                    );
            } else {
                USB_DEBUG("#USB EP0 SETUP: Recipient(%d), Type(%d), Direction(M->S), bRequest(%d) \n",setup->bmRequestType.Recipient,
                                                                                                    setup->bmRequestType.Type,
                                                                                                    setup->bRequest
                                                                                                    );
            }                                                                                                    
            switch(setup->bmRequestType.Recipient)
            {
                case USB_REQUEST_DESTINATION_DEVICE:
                {
                    USB_DEBUG("##USB dst device req.\n");
                    switch(setup->bRequest)
                    {
                        case USB_REQUEST_DEVICE_SET_ADDRESS:
                        {
                            // handled internally
                            #ifdef FEATURE_DISCONN_DETECT
                            platform_set_timer(bsp_usb_device_disconn_timeout,0);
                            #endif
                            status = USB_ERROR_NONE;
                            USB_DEBUG("###USB Set Address: 0x%04X\n", setup->wValue);
                        }
                        break;
                        case USB_REQUEST_DEVICE_CLEAR_FEATURE:
                        {
                            UsbVar.remote_wakeup = (setup->wValue&0xF) ? 0 : 1;
                            status = USB_ERROR_NONE;
                            USB_DEBUG("###USB Clear feature.\n");
                        }
                        break;
                        case USB_REQUEST_DEVICE_SET_FEATURE:
                        {
                            UsbVar.remote_wakeup = (setup->wValue&0xF) ? 1 : 0;
                            status = USB_ERROR_NONE;
                            USB_DEBUG("###USB Set Feature.\n");
                        }
                        break;
                        case USB_REQUEST_DEVICE_SET_CONFIGURATION:
                        {
                            uint8_t cfg_idx = setup->wValue&0xFF;
                            // check if the cfg_idx is correct
                            USB_DEBUG("###USB Set Configuration: cfg_idx(%d), ConfigDescriptor.config.configIndex(%d)\n", cfg_idx, ConfigDescriptor.config.configIndex);

                            if (ConfigDescriptor.config.configIndex == cfg_idx){
#if KB_DESCRIPTOR_EN                        
								status |= USB_ConfigureEp(&(ConfigDescriptor.ep_kb[0]));
#endif
#if MO_DESCRIPTOR_EN                                
                                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_mo[0]));
#endif                                
#if CTL_DESCRIPTOR_EN
                                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_ctl[0]));
                                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_ctl[1]));
#endif                                
                            } else {
                                USB_DEBUG("### cfg_idx error !!!\n");
                            }
                        }
                        break;
                        case USB_REQUEST_DEVICE_GET_DESCRIPTOR:
                        {
                            USB_DEBUG("###USB Get descriptor:%d\n", (setup->wValue >> 8));
                            switch(setup->wValue >> 8)
                            {
                                case USB_REQUEST_DEVICE_DESCRIPTOR_DEVICE:
                                {
                                    size = sizeof(USB_DEVICE_DESCRIPTOR_REAL_T);
                                    size = (setup->wLength < size) ? (setup->wLength) : size;

                                    status |= USB_SendData(0, (void*)&DeviceDescriptor, size, 0);

                                    USB_DEBUG("####USB Get descriptor: DEVICE. get_size:%d, send_size:%d, ep0MPS: %d\n", 
                                                                            (setup->wLength), size, DeviceDescriptor.ep0Mps);
                                }
                                break;
                                case USB_REQUEST_DEVICE_DESCRIPTOR_CONFIGURATION:
                                {
                                    size = sizeof(BSP_USB_DESC_STRUCTURE_T);
                                    size = (setup->wLength < size) ? (setup->wLength) : size;

                                    status |= USB_SendData(0, (void*)&ConfigDescriptor, size, 0);

                                    USB_DEBUG("####USB Get descriptor CONFIG. get_size:%d, send_size:%d\n", 
                                                                            (setup->wLength), size);
                                }
                                break;
                                case USB_REQUEST_DEVICE_DESCRIPTOR_STRING:
                                {
                                    const uint8_t *addr;
                                    switch(setup->wValue&0xFF)
                                    {
                                        case USB_STRING_LANGUAGE_IDX:
                                        {
                                            size = sizeof(StringDescriptor_0);
                                            addr = StringDescriptor_0;
                                        }break;
                                        case USB_STRING_MANUFACTURER_IDX:
                                        {
                                            size = sizeof(StringDescriptor_1);
                                            addr = StringDescriptor_1;
                                        }break;
                                        case USB_STRING_PRODUCT_IDX:
                                        {
                                            size = sizeof(StringDescriptor_2);
                                            addr = StringDescriptor_2;
                                        }break;
                                    }

                                    size = (setup->wLength < size) ? (setup->wLength) : size;
                                    status |= USB_SendData(0, (void*)addr, size, 0);

                                    USB_DEBUG("####USB Get descriptor string: index(%d), lang_id:0x%04X, get_size:%d, send_size:%d\n", 
                                                                            (setup->wValue&0xFF), setup->wIndex, setup->wLength, size);
                                }
                                break;
                                default:
                                {
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;

                                    USB_DEBUG("####USB Get descriptor unsupport: %d!!!\n", (setup->wValue >> 8));
                                }break;
                            }
                        }
                        break;
                        case USB_REQUEST_DEVICE_GET_STATUS:
                        {
                            DynamicDescriptor[0] = SELF_POWERED | (REMOTE_WAKEUP << 1);
                            DynamicDescriptor[1] = 0;
                            status |= USB_SendData(0, DynamicDescriptor, 2, 0);
                            USB_DEBUG("###USB Get status.\n");
                        }
                        break;
                        case USB_REQUEST_DEVICE_GET_CONFIGURATION:
                        {
                            DynamicDescriptor[0] = ConfigDescriptor.config.configIndex;
                            status |= USB_SendData(0, DynamicDescriptor, 1, 0);
                            USB_DEBUG("###USB Get configuration.\n");
                        }
                        break;
                        default:
                        {
                            status = USB_ERROR_REQUEST_NOT_SUPPORT;
                            USB_DEBUG("###USB dst device req unsupport !!!\n");
                        }break;
                    }
                }
                break;

                case USB_REQUEST_DESTINATION_INTERFACE:
                {
                    USB_DEBUG("##USB dst interface req.\n");
                    switch(setup->bRequest)
                    {
                        //This request is mandatory and must be supported by all devices.
                        case USB_REQUEST_HID_CLASS_REQUEST_GET_REPORT:
                        {
                            USB_DEBUG("###USB get report.\n");
                            switch(((setup->wValue)>>8)&0xFF)
                            {
                                case USB_REQUEST_HID_CLASS_REQUEST_REPORT_INPUT:
                                {
                                    USB_DEBUG("####USB REQUEST_REPORT_INPUT.\n");
                                    switch(setup->wIndex)
                                    {
#if KB_DESCRIPTOR_EN										
                                        case KB_INTERFACE_IDX:
                                        {
                                            USB_SendData(0, (void*)&KeybReport, sizeof(BSP_KEYB_REPORT_s), 0);
                                        }break;
#endif
#if MO_DESCRIPTOR_EN
                                        case MO_INTERFACE_IDX:
                                        {
                                            USB_SendData(0, (void*)&MouseReport, sizeof(BSP_MOUSE_REPORT_s), 0);
                                        }break;
#endif                                        
										default:
											USB_DEBUG("#####USB wIndex:%d, TODO\n", setup->wIndex);
											break;
                                    }
                                }break;
                                default:
                                {
                                    USB_DEBUG("####USB REQUEST_REPORT_INPUT unsupport:%d\n", (((setup->wValue)>>8)&0xFF));
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                }break;
                            }
                        }break;
                        case USB_REQUEST_HID_CLASS_REQUEST_SET_REPORT:
                        {
                            USB_DEBUG("###USB report set: req_type(0x%02X), wIndex(%d) \n", (((setup->wValue)>>8)&0xFF), setup->wIndex);
                            switch(((setup->wValue)>>8)&0xFF)
                            {
                                case USB_REQUEST_HID_CLASS_REQUEST_REPORT_OUTPUT:
                                {
                                    switch(setup->wIndex)
                                    {
                                        case KB_INTERFACE_IDX:
                                        {
#if KB_DESCRIPTOR_EN                                            
                                            // check the length, setup->wLength, for keyb, 8bit led state output is defined
                                            if (setup->wLength == 1){
                                                KeybReport.kb_led_state_recving = 1;
                                            }
                                            // refer to BSP_KEYB_KEYB_LED_e
#endif                                            
                                        }break;
                                    }
                                }break;
                                default:
                                {
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                }break;
                            }
                        }break;
                        case USB_REQUEST_HID_CLASS_REQUEST_SET_IDLE:
                        {
                            USB_DEBUG("###USB set idle: interface index(%d)\n", setup->wIndex);
                            switch(setup->wIndex)
                            {
#if KB_DESCRIPTOR_EN                                
                                case KB_INTERFACE_IDX:
                                {
                                    // KeybReport.sendBusy = U_TRUE; // set idle.
                                }break;
#endif
#if MO_DESCRIPTOR_EN                                
                                case MO_INTERFACE_IDX:
                                {
                                    // MouseReport.pending = U_TRUE; // set idle.
                                }break;
#endif                          
#if CTL_DESCRIPTOR_EN                                
                                case CTL_INTERFACE_IDX:
                                {
                                    // CtlReport.ready = U_FALSE; // set idle.
                                }break;
#endif                          
                                default:
                                    USB_DEBUG("#####USB unsupported index:%d, TODO\n", setup->wIndex);
                                    break;
                            }
                        }break;
                        case USB_REQUEST_DEVICE_GET_DESCRIPTOR:
                        {
                            USB_DEBUG("###USB get interface descriptor: HID_CLASS(0x%x), wIndex(%d)\n",(((setup->wValue)>>8)&0xFF), setup->wIndex);
                            switch(((setup->wValue)>>8)&0xFF)
                            {
                                case USB_REQUEST_HID_CLASS_DESCRIPTOR_REPORT:
                                {
                                    USB_DEBUG("####USB get report descriptor.\n");
                                    switch(setup->wIndex)
                                    {
#if KB_DESCRIPTOR_EN                                           
                                        case KB_INTERFACE_IDX:
                                        {
                                            size = sizeof(ReportKeybDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportKeybDescriptor, size, 0);
                                            KeybReport.sendBusy = U_FALSE;

                                            USB_DEBUG("#####USB Report Keyb Descriptor: get_size:%d, send_size:%d\n", setup->wLength, size);
                                        }break;
#endif
#if MO_DESCRIPTOR_EN                                           
                                        case MO_INTERFACE_IDX:
                                        {
                                            size = sizeof(ReportMouseDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportMouseDescriptor, size, 0);
                                            MouseReport.pending = U_FALSE;
											USB_DEBUG("#####USB Report Mouse Descriptor: get_size:%d, send_size:%d\n", setup->wLength, size);
                                        }break;
#endif                                        
#if CTL_DESCRIPTOR_EN
                                        case CTL_INTERFACE_IDX:
                                        {
                                            size = sizeof(ReportCtlDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportCtlDescriptor, size, 0);
                                            CtlReport.preReady = U_TRUE;
											USB_DEBUG("#####USB Report Ctl Descriptor: get_size:%d, send_size:%d\n", setup->wLength, size);
                                        }break;
#endif
                                        default:
                                        {
                                            USB_DEBUG("###USB unsupport report descriptor:%d, TODO\n", setup->wIndex);
                                            status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                        }break;
                                    }
                                }break;
                                default:
                                {
                                    USB_DEBUG("###USB unsupport interface descriptor:0x%02X!!!, TODO\n", ((setup->wValue)>>8)&0xFF);
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                }break;
                            }
                        }
                        break;
                        default:
                        {
                            USB_DEBUG("###USB dst interface req unsupport !!!, TODO\n");
                            status = USB_ERROR_REQUEST_NOT_SUPPORT;
                        }break;
                    }
                }
                break;
                default:
                {
                    USB_DEBUG("###USB Recipient unsupport !!!, TODO\n");
                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
            }

            // if status equals to USB_ERROR_REQUEST_NOT_SUPPORT: it is unsupported request.
            // if status equals to USB_ERROR_NONE: it is successfully executed.
            if((USB_ERROR_NONE != status) && (USB_ERROR_REQUEST_NOT_SUPPORT != status))
            {
                USB_DEBUG("USB event exec error %x (0x%x 0x%x)\n", status, *(uint32_t*)setup,*((uint32_t*)setup+1));
            }
        }break;

        case USB_EVENT_EP_DATA_TRANSFER:
        {
            USB_DEBUG("#USB ep DATA TRANSFER\n");
            switch(event->data.type)
            {
                case USB_CALLBACK_TYPE_RECEIVE_END:
                {
                    USB_DEBUG("##USB RECV END: ep(%d)\n", event->data.ep);
                    // Endpoint 0 received data.
                    if (event->data.ep == 0){
                        uint8_t *data = (uint8_t *)USB_GetEp0SetupData();
                        if (KeybReport.kb_led_state_recving){
                            KeybReport.kb_led_state_recving = 0;
                            KeybReport.led_state = data[0];
                        }
                    }

#if CTL_DESCRIPTOR_EN
                    if(event->data.ep == EP_CTL_OUT){
                        CtlReport.ready = U_TRUE;

                        #if 0
                        platform_printf("(%d)RECV[%d]: ",event->data.ep, event->data.size);printf_hexdump(DataRecvBuf, event->data.size);
                        #endif

                        /* Push rx data to user callback. */
                        if (CTL_REPORT_ID == DataRecvBuf[0]){
                            bsp_usb_hid_ctl_push_rx_data_to_user(&DataRecvBuf[1], event->data.size-1);
                        } else if(KB_REPORT_ID == DataRecvBuf[0]){
                            platform_printf("KB RECV[%d]: ", event->data.size-1);
                            printf_hexdump(&DataRecvBuf[1], event->data.size);
                        }

                        /* Start next rx proc. */
                        bsp_usp_hid_ctl_rx_data_trigger(2);
                    }
#endif
                }break;
                case USB_CALLBACK_TYPE_TRANSMIT_END:
                {
					USB_DEBUG("##USB send OK: ep(%d)\n", event->data.ep);
#if CTL_DESCRIPTOR_EN
					/* Enter receiving status after setup complete. */
                    if(event->data.ep == 0 && CtlReport.preReady == U_TRUE){
                        CtlReport.preReady = U_FALSE;

                        /* Start first rx proc. */
                        bsp_usp_hid_ctl_rx_data_trigger(1);

                        CtlReport.ready = U_TRUE; // 枚举结束
                        platform_printf("===> USB OK <===\n");
                    }
#endif                    
					
					/* If send ok, Clear busy status, and notify user. */
                    switch(event->data.ep)
                    {
#if KB_DESCRIPTOR_EN                        
                        case EP_KB_IN:
                        {
                            KeybReport.sendBusy = U_FALSE;
                        }break;
#endif
#if MO_DESCRIPTOR_EN                        
                        case EP_MO_IN:
                        {
                            MouseReport.pending = U_FALSE;
                        }break;
#endif                        
#if CTL_DESCRIPTOR_EN
						case EP_CTL_IN:
                        {
                            CtlReport.sendBusy = U_FALSE;
                            bsp_usb_hid_ctl_push_send_complete_to_user();
                        }break;
#endif
                    }

                }break;
                default:
                    USB_DEBUG("##USB unsupport type:%d, TODO\n", event->data.type);
                    break;
            }
        }break;
        default:
            USB_DEBUG("#USB unsupport id:%d, TODO\n", event->id);
            break;
    }

    return status;
}

// ===================================================================================================
#if KB_DESCRIPTOR_EN

// check basic report busy status.
static USB_HID_BusySta_t bsp_usb_hid_keyboard_basic_report_status_get(void){
    if (U_TRUE == KeybReport.sendBusy){
        return USB_STA_BUSY;
    }
    return USB_STA_IDLE;
}

// trigger sending of basic key value.
static USB_HID_OperateSta_t bsp_usb_hid_keyboard_basic_report_start(void){
    if (USB_STA_BUSY == bsp_usb_hid_keyboard_basic_report_status_get()){
        return USB_HID_ERROR_BUSY;
    }
    USB_SendData(USB_EP_DIRECTION_IN(EP_KB_IN), (void*)&(KeybReport.report), sizeof(BSP_KEYB_REPORT_s), 0);
    KeybReport.sendBusy = U_TRUE;
    return USB_HID_ERROR_NONE;
}

static void bsp_usb_hid_kb_basic_report_general_key_reorder(uint8_t spot_index){
    if (spot_index >= (KEY_TABLE_LEN-1)){
        return;
    }
    for(uint8_t i = spot_index; i < (KEY_TABLE_LEN-1); i++){
        if (KeybReport.report.key_table[i+1] != 0x00){
            KeybReport.report.key_table[i] = KeybReport.report.key_table[i+1];
            KeybReport.report.key_table[i+1] = 0x00;
        }
    }
    return;
}

// set basic general key value.
static uint8_t bsp_usb_hid_keyboard_basic_report_set_general_key_value(uint8_t key, uint8_t press){
    uint8_t index;
    if(press){
        for(index = 0; index < KEY_TABLE_LEN; index++){
            if(key == KeybReport.report.key_table[index]){
                // already pressed
                return U_FAIL;
            }
        }
        for(index = 0; index < KEY_TABLE_LEN; index++){
            if(0x00 == KeybReport.report.key_table[index]){
                // find first empty spot, populate it
                KeybReport.report.key_table[index] = key;
                return U_SUCCESS; // effective change
            }
        }
        // no empty spot, return
        if(index == KEY_TABLE_LEN){
            USB_DEBUG("ERROR: no empty spot, never come here!\n");
            return U_FAIL;
        }
    } else {
        for(index = 0; index < KEY_TABLE_LEN; index++){
            if(key == KeybReport.report.key_table[index]){
                // already pressed, clear it
                KeybReport.report.key_table[index] = 0x00;
                bsp_usb_hid_kb_basic_report_general_key_reorder(index);
                return U_SUCCESS; // effective change

            }
        }
        if(index == KEY_TABLE_LEN){
            // USB_DEBUG("ERROR: not find the key.\n");
            return U_FAIL; // No effective change
        }
    }
    return U_FAIL;
}

// set basic modifier key value.
static void bsp_usb_hid_keyboard_basic_report_set_modifier_key_value(BSP_KEYB_KEYB_MODIFIER_e modifier, uint8_t press){
    if(press){
        KeybReport.report.modifier |= modifier;
    } else {
        KeybReport.report.modifier &= ~modifier;
    }
}

// get basic report key press count.
static uint8_t bsp_usb_hid_kb_get_basic_key_cnt(void){
    uint8_t index;
    for(index = 0; index < KEY_TABLE_LEN; index++){
        if(0x00 == KeybReport.report.key_table[index]){
            return index;
        }
    }
    return KEY_TABLE_LEN;
}

#if 0
// ================================================================================================
void bsp_usb_handle_hid_keyb_key_report(uint8_t key, uint8_t press)
{
    uint32_t j;
    if(U_FALSE == KeybReport.sendBusy)
    {
        if(press)
        {
            for(j = 0; j < KEY_TABLE_LEN; j++)
            {
                if(key == KeybReport.report.key_table[j])
                {
                    // already pressed
                    return;
                }
            }
            for(j = 0; j < KEY_TABLE_LEN; j++)
            {
                if(0 == KeybReport.report.key_table[j])
                {
                    // find first empty spot, populate it
                    KeybReport.report.key_table[j] = key;
                    break;
                }
            }
            // no empty spot, return
            if(j == KEY_TABLE_LEN){return;}
        }
        else
        {
            for(j = 0; j < KEY_TABLE_LEN; j++)
            {
                if(key == KeybReport.report.key_table[j])
                {
                    // already pressed, clear it
                    KeybReport.report.key_table[j] = 0x00;
                    break;
                }
            }
            if(j == KEY_TABLE_LEN){return;}
        }

        USB_SendData(USB_EP_DIRECTION_IN(EP_KB_IN), (void*)&(KeybReport.report), sizeof(BSP_KEYB_REPORT_s), 0);
        KeybReport.sendBusy = U_TRUE;
    }
}

void bsp_usb_handle_hid_keyb_modifier_report(BSP_KEYB_KEYB_MODIFIER_e modifier, uint8_t press)
{
    uint8_t last_press_state = ((KeybReport.report.modifier & modifier) != 0);
    if((U_FALSE == KeybReport.sendBusy)&&(last_press_state != press))
    {
        if(press)
        {
            KeybReport.report.modifier |= modifier;
        }
        else
        {
            KeybReport.report.modifier &= ~modifier;
        }

        USB_SendData(USB_EP_DIRECTION_IN(EP_KB_IN), (void*)&(KeybReport.report), sizeof(BSP_KEYB_REPORT_s), 0);
        KeybReport.sendBusy = U_TRUE;
    }
}
#endif

uint8_t bsp_usb_get_hid_keyb_led_report(void)
{
    return (KeybReport.led_state);
}

static void bsp_usb_handle_hid_keyb_clear_report_buffer(void)
{
    memset(&(KeybReport.report), 0x00, sizeof(BSP_KEYB_REPORT_s));
}
#endif // #if KB_DESCRIPTOR_EN

// ===================================================================================================
#if MO_DESCRIPTOR_EN
void bsp_usb_handle_hid_mouse_report(int8_t x, int8_t y, uint8_t btn)
{
    if((U_FALSE == MouseReport.pending)&&((0!=x)||(0!=y)||(btn!=MouseReport.report.button)))
    {
        MouseReport.report.pos_x = x;
        MouseReport.report.pos_y = y;
        MouseReport.report.button = btn;

        USB_SendData(USB_EP_DIRECTION_IN(EP_MO_IN), (void*)&MouseReport, sizeof(BSP_MOUSE_REPORT_s), 0);
        MouseReport.pending = U_TRUE;
    }
}

void bsp_usb_handle_hid_mouse_clear_report_buffer(void)
{
    memset(&(MouseReport.report),0x00, sizeof(BSP_MOUSE_REPORT_s));
}
#endif // #if MO_DESCRIPTOR_EN

// ===================================================================================================
#if CTL_DESCRIPTOR_EN

static void bsp_usb_hid_ctl_push_rx_data_to_user(uint8_t *data, uint16_t len){
    if (usb_hid_ctl_recv_callback){
        usb_hid_ctl_recv_callback(data, len);
    }
}

static void bsp_usb_hid_ctl_push_send_complete_to_user(void){
    if (usb_hid_ctl_send_complete_callback){
        usb_hid_ctl_send_complete_callback();
    }
}

static USB_ERROR_TYPE_E bsp_usp_hid_ctl_rx_data_trigger(uint8_t printFLAG){
    USB_DEBUG("===> RECVING(%d) ...\n", printFLAG);
    memset(DataRecvBuf, 0x00, sizeof(DataRecvBuf));
    return USB_RecvData(ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_OUT)].ep, DataRecvBuf, ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_OUT)].mps, 0);
}

static USB_ERROR_TYPE_E bsp_usp_hid_ctl_tx_data_trigger(uint8_t reportID, uint8_t *data, uint16_t len){
    USB_DEBUG("===> Sending[RID=0x%02X] ...\n", reportID);
    DataSendBuf[0] = reportID;
    memcpy(&DataSendBuf[1], data, len);
    uint8_t size = len+1;
    size = (size <= ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_IN)].mps) ? (size) : (ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_IN)].mps);
    return USB_SendData(ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_IN)].ep, DataSendBuf, size, 0);
}

static USB_HID_OperateSta_t bsp_usb_hid_report_send(uint8_t reportID, uint8_t *data, uint16_t len){

    if(!CtlReport.ready){
        return USB_HID_ERROR_NOT_READY;
    }

    if(CtlReport.sendBusy){
        return USB_HID_ERROR_BUSY;
    }

    if( data == NULL || len == 0 || len > MAX_REPORT_SIZE ){
        return USB_HID_ERROR_INVALID_PARAM;
    }

    USB_HID_OperateSta_t error = USB_HID_ERROR_NONE;

    USB_ERROR_TYPE_E status = bsp_usp_hid_ctl_tx_data_trigger(reportID, data, len);
    switch(status){
        case USB_ERROR_NONE:
            CtlReport.sendBusy = U_TRUE;
            break;
        case USB_ERROR_INVALID_INPUT:
            error = USB_HID_ERROR_INTERNAL_ERR;
            break;
        case USB_ERROR_INACTIVE_EP:
            error = USB_HID_ERROR_INACTIVE_EP;
            break;
        default:
            error = USB_HID_ERROR_UNKNOW_ERR;
            break;
    }

    return error;
}

USB_HID_OperateSta_t bsp_usb_hid_ctl_send(uint8_t *data, uint16_t len){
    len = ConfigDescriptor.ep_ctl[EP_CTL_IDX_GET(EP_CTL_IN)].mps-1;
    return bsp_usb_hid_report_send(CTL_REPORT_ID, data, len);
}

void bsp_usb_hid_ctl_send_complete_callback_register(bsp_usb_hid_ctl_send_complete_cb_t cb){
    usb_hid_ctl_send_complete_callback = cb;
}

void bsp_usb_hid_ctl_recv_callback_register(bsp_usb_hid_ctl_recv_cb_t cb){
    usb_hid_ctl_recv_callback = cb;
}

#if KB_EXT_DESCRP_EN
// For keyboard.
static USB_HID_OperateSta_t bsp_usb_hid_send_ext_key(uint8_t *data, uint16_t len){
    return bsp_usb_hid_report_send(KB_REPORT_ID, data, len);
}

USB_HID_BusySta_t bsp_usb_hid_keyboard_extend_report_status_get(void){
    if (U_TRUE == CtlReport.sendBusy){
        return USB_STA_BUSY;
    }
    return USB_STA_IDLE;
}

USB_HID_OperateSta_t bsp_usb_hid_keyboard_extend_report_start(void){
    return bsp_usb_hid_send_ext_key(KeybReport.ext_key_table, sizeof(KeybReport.ext_key_table));
}

/**
 * @brief set extend key value.
 * @param key 
 * @param press 
 * @return uint8_t   0:No effective change,  1:effective change
 */
uint8_t bsp_usb_hid_keyboard_extend_report_set_key_value(uint8_t key, uint8_t press){
    uint8_t index = ((key - HID_KEYB_A) / 8);
    uint8_t offset = ((key - HID_KEYB_A) % 8);
    if (press){
        if (!(KeybReport.ext_key_table[index] & (1<<offset))) {
            KeybReport.ext_key_table[index] |= (1<<offset);
            return U_SUCCESS; // The key released and press it successfully.
        }
    } else {
        if (KeybReport.ext_key_table[index] & (1<<offset)){
            KeybReport.ext_key_table[index] &= ~(1<<offset);
            return U_SUCCESS; // The key pressed and release it successfully.
        }
    }
    return U_FAIL; // No effective change
}

// ===================================================================================================
#if USE_SOF_TRIGGER_KB_SEND_EN
#else
static bsp_usb_hid_kb_basic_delay_send_cb_t  kb_basic_delay_send_callback = NULL;
static bsp_usb_hid_kb_extend_delay_send_cb_t kb_extend_delay_send_callback = NULL;

static uint8_t bsp_usb_hid_kb_basic_report_delay_send_trigger(void){
    if (kb_basic_delay_send_callback){
        kb_basic_delay_send_callback();
        return U_SUCCESS;
    }
    return U_FAIL;
}
static uint8_t bsp_usb_hid_kb_extend_report_delay_send_trigger(void){
    if (kb_extend_delay_send_callback){
        kb_extend_delay_send_callback();
        return U_SUCCESS;
    }
    return U_FAIL;
}

void bsp_usb_hid_kb_basic_delay_send_callback_register(bsp_usb_hid_kb_basic_delay_send_cb_t cb){
    kb_basic_delay_send_callback = cb;
}
void bsp_usb_hid_kb_extend_delay_send_callback_register(bsp_usb_hid_kb_extend_delay_send_cb_t cb){
    kb_extend_delay_send_callback = cb;
}
#endif

// check all key released.
static uint8_t bsp_usb_hid_kb_all_key_release_check(void){
    uint8_t index;
    if(KeybReport.report.modifier != 0x00){
        return U_FALSE;
    }
    for(index=0; index<KEY_TABLE_LEN; index++){
        if(KeybReport.report.key_table[index] != 0x00){
            return U_FALSE;
        }
    }
    for(index=0; index<EXT_KEY_TABLE_LEN; index++){
        if(KeybReport.ext_key_table[index] != 0x00){
            return U_FALSE;
        }
    }
    return U_TRUE;
}

static void bsp_usb_hid_kb_send_basic_key_trigger(void){
#if USE_SOF_TRIGGER_KB_SEND_EN
    KeybReport.basic_send_flag = U_TRUE;
#else
    // send start.
    USB_HID_OperateSta_t status = bsp_usb_hid_keyboard_basic_report_start();
    if (USB_HID_ERROR_NONE == status){
    } else if (USB_HID_ERROR_BUSY == status){
        if(bsp_usb_hid_kb_basic_report_delay_send_trigger() == U_FAIL){
            USB_ERROR("ERROR: The usb is busy, and the basic delayed send function is not registered, so the key value is discarded!\n");
        }
    } else {
        USB_ERROR("hid basic key send errror:%d\n", status);
    }
#endif
}

static void bsp_usb_hid_kb_send_extend_key_trigger(void){
#if USE_SOF_TRIGGER_KB_SEND_EN
    KeybReport.extend_send_flag = U_TRUE;
#else
    // send start.
    USB_HID_OperateSta_t status = bsp_usb_hid_keyboard_extend_report_start();
    if (USB_HID_ERROR_NONE == status){
    } else if (USB_HID_ERROR_BUSY == status){
        if(bsp_usb_hid_kb_extend_report_delay_send_trigger() == U_FAIL){
            USB_ERROR("ERROR: The usb is busy, and the extend delayed send function is not registered, so the key value is discarded!\n");
        }
    } else {
        USB_ERROR("hid extend key send errror:%d\n", status);
    }
#endif
}

// send key.
void bsp_usb_hid_kb_key_report(BSP_HID_KB_Type_t type, uint8_t key, uint8_t press){

    if (press){

        /* press : send key. */
        if (type == KEY_TYPE_MODIFIER){
            // update basic modifier key.
            bsp_usb_hid_keyboard_basic_report_set_modifier_key_value((BSP_KEYB_KEYB_MODIFIER_e)key, press);
            // trigger basic send.
            bsp_usb_hid_kb_send_basic_key_trigger();
        } else {
            if (KeybReport.extend_flag){
                // find the extend key and update it's value.
                bsp_usb_hid_keyboard_extend_report_set_key_value(key, press);
                // trigger extend send.
                bsp_usb_hid_kb_send_extend_key_trigger();
            } else {
                if(bsp_usb_hid_kb_get_basic_key_cnt() >= KEY_TABLE_LEN){
                    // update extend flag.
                    KeybReport.extend_flag = U_TRUE;
                    // find the extend key and update it's value.
                    bsp_usb_hid_keyboard_extend_report_set_key_value(key, press);
                    // trigger extend send.
                    bsp_usb_hid_kb_send_extend_key_trigger();
                } else {
                    // find the basic general key and update it's value.
                    bsp_usb_hid_keyboard_basic_report_set_general_key_value(key, press);
                    // trigger basic send.
                    bsp_usb_hid_kb_send_basic_key_trigger();
                }
            }
        }
    } else {

        /* release : send key. */
        if (type == KEY_TYPE_MODIFIER){
            // update basic modifier key.
            bsp_usb_hid_keyboard_basic_report_set_modifier_key_value((BSP_KEYB_KEYB_MODIFIER_e)key, press);
            // trigger basic send.
            bsp_usb_hid_kb_send_basic_key_trigger();
        } else {
            // find the basic general key and update it's value.
            if (bsp_usb_hid_keyboard_basic_report_set_general_key_value(key, press) == U_SUCCESS){
                // trigger basic send.
                bsp_usb_hid_kb_send_basic_key_trigger();
            } else {
                // find the extend key and update it's value.
                if (bsp_usb_hid_keyboard_extend_report_set_key_value(key, press) == U_SUCCESS){
                    // trigger extend send.
                    bsp_usb_hid_kb_send_extend_key_trigger();
                } else {
                    // Invalid key value, discarded. Do nothing.
                    USB_ERROR("ERRROR: Invalid key value, discarded!\n");
                }
            }
        }
        
        // all key release check.
        if(bsp_usb_hid_kb_all_key_release_check() == U_TRUE){
            KeybReport.extend_flag = U_FALSE;
            USB_ERROR("ALL RELEASE\n");
        }

    }
}

#endif // #if KB_EXT_DESCRP_EN



#endif

static void usb_reset_keyboard_init(void){
    memset(&KeybReport, 0, sizeof(KeybReport));
    memset(&CtlReport, 0, sizeof(CtlReport));
}

#if USE_SOF_TRIGGER_KB_SEND_EN
static void usb_sof_exist_check(void){
    static uint32_t sof_cnt = 0;
    // static uint32_t flag_1s = 0;
    sof_cnt++;
    if(sof_cnt % 1000 == 0){
        // flag_1s++;
        // if(flag_1s == 5){
        //     bsp_usb_hid_kb_key_report(KEY_TYPE_MODIFIER, HID_KEYB_MODIFIER_LEFT_SHIFT, 1);
        //     bsp_usb_hid_kb_key_report(KEY_TYPE_MODIFIER, HID_KEYB_MODIFIER_RIGHT_CTRL, 1);
        // } else if(flag_1s == 6){
        //     bsp_usb_hid_kb_key_report(KEY_TYPE_MODIFIER, HID_KEYB_MODIFIER_LEFT_SHIFT, 0);
        //     bsp_usb_hid_kb_key_report(KEY_TYPE_MODIFIER, HID_KEYB_MODIFIER_RIGHT_CTRL, 0);
        // }
        platform_printf("sof.\n");
    }
}

static void usb_sof_keyboard_send_check(void){

    /* Check basic key sending. */
    if (KeybReport.basic_send_flag){
        // send basic start.
        USB_HID_OperateSta_t status = bsp_usb_hid_keyboard_basic_report_start();
        if (USB_HID_ERROR_NONE == status){
            KeybReport.basic_send_flag = U_FALSE;
        } else if (USB_HID_ERROR_BUSY == status){
            // busy, wait next cycle.
        } else {
            USB_ERROR("hid basic key unknow errror:%d\n", status);
            KeybReport.basic_send_flag = U_FALSE; //clear
        }
    }

    /* Check extend key sending. */
    if (KeybReport.extend_send_flag){
        // send extend start.
        USB_HID_OperateSta_t status = bsp_usb_hid_keyboard_extend_report_start();
        if (USB_HID_ERROR_NONE == status){
            KeybReport.extend_send_flag = U_FALSE;
        } else if (USB_HID_ERROR_BUSY == status){
            // busy, wait next cycle.
        } else {
            USB_ERROR("hid extend key unknow errror:%d\n", status);
            KeybReport.basic_send_flag = U_FALSE; //clear
        }
    }

    /* check sof run status. */
    // usb_sof_exist_check();

}
#endif

// ===================================================================================================
// ===================================================================================================
void bsp_usb_init(void)
{
    USB_INIT_CONFIG_T config;

    SYSCTRL_ClearClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);
    //use SYSCTRL_GetClk(SYSCTRL_ITEM_APB_USB) to confirm, USB module clock has to be 48M.
    SYSCTRL_SelectUSBClk((SYSCTRL_ClkMode)(SYSCTRL_GetPLLClk()/48000000));

    platform_set_irq_callback(PLATFORM_CB_IRQ_USB, USB_IrqHandler, NULL);

    PINCTRL_SelUSB(USB_PIN_DP,USB_PIN_DM);

    SYSCTRL_USBPhyConfig(BSP_USB_PHY_ENABLE,BSP_USB_PHY_DP_PULL_UP);

    memset(&config, 0x00, sizeof(USB_INIT_CONFIG_T));
    config.intmask = USBINTMASK_SUSP | USBINTMASK_RESUME | USBINTMASK_SOF;
    config.handler = bsp_usb_event_handler;
    USB_InitConfig(&config);
}

void bsp_usb_disable(void)
{
    USB_Close();
    SYSCTRL_SetClkGateMulti(1 << SYSCTRL_ITEM_APB_USB);

    SYSCTRL_USBPhyConfig(BSP_USB_PHY_DISABLE,0);
}

static void internal_bsp_usb_device_remote_wakeup_stop(void)
{
    USB_DeviceSetRemoteWakeupBit(U_FALSE);
}

void bsp_usb_device_remote_wakeup(void)
{
    USB_DeviceSetRemoteWakeupBit(U_TRUE);
    platform_set_timer(internal_bsp_usb_device_remote_wakeup_stop,16);// setup timer for 10ms, then disable resume signal
}

#ifdef FEATURE_DISCONN_DETECT
void bsp_usb_device_disconn_timeout(void)
{
    bsp_usb_disable();
    USB_DEBUG("USB cable disconnected.");
}
#endif

