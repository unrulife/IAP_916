#include <stdio.h>
#include <string.h>
#include "ingsoc.h"
#include "platform_api.h"
#include "bsp_usb_hid_kb.h"
#include "btstack_util.h"

#if 1
#define USB_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define USB_DEBUG(...)      
#endif

const USB_DEVICE_DESCRIPTOR_REAL_T DeviceDescriptor __attribute__ ((aligned (4))) = USB_DEVICE_DESCRIPTOR;
const BSP_USB_DESC_STRUCTURE_T ConfigDescriptor __attribute__ ((aligned (4))) =
{
    USB_CONFIG_DESCRIPTOR,
    USB_INTERFACE_DESCRIPTOR_KB,  USB_HID_DESCRIPTOR_KB,  {USB_EP_IN_DESCRIPTOR_KB},
    USB_INTERFACE_DESCRIPTOR_MO,  USB_HID_DESCRIPTOR_MO,  {USB_EP_IN_DESCRIPTOR_MO},
    USB_INTERFACE_DESCRIPTOR_CTL, USB_HID_DESCRIPTOR_CTL, {USB_EP_IN_DESCRIPTOR_CTL, USB_EP_OUT_DESCRIPTOR_CTL}
};
const uint8_t StringDescriptor_0[] __attribute__ ((aligned (4))) = USB_STRING_LANGUAGE;
const uint8_t StringDescriptor_1[] __attribute__ ((aligned (4))) = USB_STRING_MANUFACTURER;
const uint8_t StringDescriptor_2[] __attribute__ ((aligned (4))) = USB_STRING_PRODUCT;
uint8_t DynamicDescriptor[64] __attribute__ ((aligned (4)));
const uint8_t ReportMouseDescriptor[] __attribute__ ((aligned (4))) = USB_HID_MOUSE_REPORT_DESCRIPTOR;
const uint8_t ReportKeybDescriptor[] __attribute__ ((aligned (4))) = USB_HID_KB_REPORT_DESCRIPTOR;
const uint8_t ReportCtlDescriptor[] __attribute__ ((aligned (4))) = USB_HID_CTL_REPORT_DESCRIPTOR;

BSP_USB_VAR_s UsbVar;
uint8_t DataRecvBuf[EP_CTL_MPS_BYTES] __attribute__ ((aligned (4)));
uint8_t DataSendBuf[EP_CTL_MPS_BYTES] __attribute__ ((aligned (4)));

BSP_KEYB_DATA_s KeybReport __attribute__ ((aligned (4))) = {.pending = U_TRUE};
BSP_MOUSE_DATA_s MouseReport __attribute__ ((aligned (4))) = {.pending = U_TRUE};

BSP_CTL_DATA_s CtlReport __attribute__ ((aligned (4))) = {
    .pending = U_TRUE, 
    .preReady = U_FALSE,
    .ready = U_FALSE,
    .sendBusy = U_FALSE
};

static bsp_usb_hid_ctl_recv_cb_t              usb_hid_ctl_recv_callback = NULL;
static bsp_usb_hid_ctl_send_complete_cb_t     usb_hid_ctl_send_complete_callback = NULL;


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
    return USB_RecvData(ConfigDescriptor.ep_ctl[EP_CTL_OUT-3].ep, DataRecvBuf, ConfigDescriptor.ep_ctl[EP_CTL_OUT-3].mps, 0);
}

static USB_ERROR_TYPE_E bsp_usp_hid_ctl_tx_data_trigger(uint8_t printFLAG, uint8_t *data, uint16_t len){
    USB_DEBUG("===> Sending(%d) ...\n", printFLAG);
    DataSendBuf[0] = CTL_REPORT_ID;
    memcpy(&DataSendBuf[1], data, len);
    return USB_SendData(ConfigDescriptor.ep_ctl[EP_CTL_IN-3].ep, DataSendBuf, ConfigDescriptor.ep_ctl[EP_CTL_IN-3].mps, 0);
}

static uint32_t bsp_usb_event_handler(USB_EVNET_HANDLER_T *event)
{
    uint32_t size;
    uint32_t status = USB_ERROR_NONE;

    USB_DEBUG("\n\n------------- evt_id:%d ------------\n", event->id);

    switch(event->id)
    {
        case USB_EVENT_DEVICE_RESET:
        {
            #ifdef FEATURE_DISCONN_DETECT
            platform_set_timer(bsp_usb_device_disconn_timeout,160);
            #endif
            USB_DEBUG("#USB RESET\n");
        }break;
        case USB_EVENT_DEVICE_SOF:
        {
            USB_DEBUG("#USB SOF\n");
            // handle sof, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_DEVICE_SUSPEND:
        {
            USB_DEBUG("#USB SUSPEND\n");
            CtlReport.ready = U_FALSE;
            // handle suspend, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_DEVICE_RESUME:
        {
            USB_DEBUG("#USB RESUME\n");
            CtlReport.ready = U_TRUE;
            // handle resume, need enable interrupt in config.intmask
        }break;
        case USB_EVENT_EP0_SETUP:
        {

            USB_SETUP_T* setup = USB_GetEp0SetupData();

            USB_DEBUG("#USB EP0 SETUP: Recipient(%d), Type(%d), Direction(%d), bRequest(%d) \n",setup->bmRequestType.Recipient,
                                                                                                setup->bmRequestType.Type,
                                                                                                setup->bmRequestType.Direction,
                                                                                                setup->bRequest
                                                                                                );
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
                            USB_DEBUG("###USB Set Address.\n");
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
								status |= USB_ConfigureEp(&(ConfigDescriptor.ep_kb[0]));
                            	status |= USB_ConfigureEp(&(ConfigDescriptor.ep_mo[0]));
                                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_ctl[0]));
                                status |= USB_ConfigureEp(&(ConfigDescriptor.ep_ctl[1]));
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

                                    USB_DEBUG("####USB Get descriptor device.\n");
                                }
                                break;
                                case USB_REQUEST_DEVICE_DESCRIPTOR_CONFIGURATION:
                                {
                                    size = sizeof(BSP_USB_DESC_STRUCTURE_T);
                                    size = (setup->wLength < size) ? (setup->wLength) : size;

                                    status |= USB_SendData(0, (void*)&ConfigDescriptor, size, 0);

                                    USB_DEBUG("####USB Get descriptor configuration.\n");
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

                                    USB_DEBUG("####USB Get descriptor string: wValue(%d)\n", (setup->wValue&0xFF));

                                    // if((setup->wValue&0xFF) == 2){
                                    //     CtlReport.preReady = U_TRUE;
                                    // }
                                }
                                break;
                                default:
                                {
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;

                                    USB_DEBUG("####USB Get descriptor not support: %d!!!\n", (setup->wValue >> 8));
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
                            USB_DEBUG("###USB dst device req not support !!!\n");
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
										
                                        case 0:
                                        {
                                            USB_SendData(0, (void*)&KeybReport, sizeof(BSP_KEYB_REPORT_s), 0);
                                        }break;
                                        case 1:
                                        {
                                            USB_SendData(0, (void*)&MouseReport, sizeof(BSP_MOUSE_REPORT_s), 0);
                                        }break;
										default:
											USB_DEBUG("#####USB wIndex:%d, TODO\n", setup->wIndex);
											break;
                                    }
                                }break;
                                default:
                                {
                                    USB_DEBUG("####USB REQUEST_REPORT_INPUT not support:%d\n", (((setup->wValue)>>8)&0xFF));
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                }break;
                            }
                        }break;
                        case USB_REQUEST_HID_CLASS_REQUEST_SET_REPORT:
                        {
                            USB_DEBUG("###USB set report: wValue(%d), wIndex(%d), TODO\n",(((setup->wValue)>>8)&0xFF), setup->wIndex);
                            switch(((setup->wValue)>>8)&0xFF)
                            {
                                case USB_REQUEST_HID_CLASS_REQUEST_REPORT_OUTPUT:
                                {
                                    switch(setup->wIndex)
                                    {
                                        case 0:
                                        {
                                            // check the length, setup->wLength, for keyb, 8bit led state output is defined
                                            KeybReport.led_state = setup->data[0];
                                            // refer to BSP_KEYB_KEYB_LED_e
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
                            USB_DEBUG("###USB set idle: wIndex(%d)\n", setup->wIndex);
                            switch(setup->wIndex)
                            {
                                case 0:
                                {
                                    KeybReport.pending = U_TRUE;
                                }break;
                                case 1:
                                {
                                    MouseReport.pending = U_TRUE;
                                }break;
                            }
                        }break;
                        case USB_REQUEST_DEVICE_GET_DESCRIPTOR:
                        {
                            USB_DEBUG("###USB get descriptor: wValue(0x%x), wIndex(%d)\n",(((setup->wValue)>>8)&0xFF), setup->wIndex);
                            switch(((setup->wValue)>>8)&0xFF)
                            {
                                case USB_REQUEST_HID_CLASS_DESCRIPTOR_REPORT:
                                {
                                    switch(setup->wIndex)
                                    {
                                        case 0:
                                        {
                                            size = sizeof(ReportKeybDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportKeybDescriptor, size, 0);
                                            KeybReport.pending = U_FALSE;
                                        }break;
                                        case 1:
                                        {
                                            size = sizeof(ReportMouseDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportMouseDescriptor, size, 0);
                                            MouseReport.pending = U_FALSE;
                                        }break;
                                        case 3:
                                        {
                                            size = sizeof(ReportCtlDescriptor);
                                            size = (setup->wLength < size) ? (setup->wLength) : size;

                                            status |= USB_SendData(0, (void*)&ReportCtlDescriptor, size, 0);
                                        }break;
                                    }
                                }break;
                                default:
                                {
                                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                                }break;
                            }
                        }
                        break;
                        default:
                        {
                            USB_DEBUG("###USB dst interface req not support !!!\n");
                            status = USB_ERROR_REQUEST_NOT_SUPPORT;
                        }break;
                    }
                }
                break;
                default:
                {
                    USB_DEBUG("###USB Recipient not support !!!\n");
                    status = USB_ERROR_REQUEST_NOT_SUPPORT;
                }break;
            }

            // if status equals to USB_ERROR_REQUEST_NOT_SUPPORT: it is not supported request.
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
                    USB_DEBUG("##USB recv end: ep(%d)\n", event->data.ep);
                    if(event->data.ep == EP_CTL_OUT){

                        #if 0
                        platform_printf("(%d)RECV[%d]: ",event->data.ep, event->data.size);printf_hexdump(DataRecvBuf, event->data.size);
                        #endif

                        // /* Push rx data to user callback. */
                        // bsp_usb_hid_ctl_push_rx_data_to_user(&DataRecvBuf[1], event->data.size-1);

                        // /* Start next rx proc. */
                        // bsp_usp_hid_ctl_rx_data_trigger(2);
                    }
                }break;
                case USB_CALLBACK_TYPE_TRANSMIT_END:
                {
					USB_DEBUG("##USB send OK: ep(%d)\n", event->data.ep);
					
					// /* Enter receiving status after setup complete. */
                    // if(event->data.ep == 0 && CtlReport.preReady == U_TRUE){
                    //     CtlReport.preReady = U_FALSE;

                    //     /* Start first rx proc. */
                    //     bsp_usp_hid_ctl_rx_data_trigger(1);

                    //     CtlReport.ready = U_TRUE; // 枚举结束
                    //     platform_printf("===> USB OK <===\n");
                    // }
					
					/* If send ok, Clear busy status, and notify user. */
                    switch(event->data.ep)
                    {
                        case EP_KB_IN:
                        {
                            KeybReport.pending = U_FALSE;
                        }break;
                        case EP_MO_IN:
                        {
                            MouseReport.pending = U_FALSE;
                        }break;
						case EP_CTL_IN:
                        {
                            CtlReport.sendBusy = U_FALSE;
                            // bsp_usb_hid_ctl_push_send_complete_to_user();
                        }break;
                    }

                }break;
                default:
                    USB_DEBUG("##USB unsupport type:%d\n", event->data.type);
                    break;
            }
        }break;
        default:
            USB_DEBUG("#USB unsupport id:%d\n", event->id);
            break;
    }

    return status;
}
void bsp_usb_handle_hid_keyb_key_report(uint8_t key, uint8_t press)
{
    uint32_t j;
    if(U_FALSE == KeybReport.pending)
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
        KeybReport.pending = U_TRUE;
    }
}

void bsp_usb_handle_hid_keyb_modifier_report(BSP_KEYB_KEYB_MODIFIER_e modifier, uint8_t press)
{
    uint8_t last_press_state = ((KeybReport.report.modifier & modifier) != 0);
    if((U_FALSE == KeybReport.pending)&&(last_press_state != press))
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
        KeybReport.pending = U_TRUE;
    }
}

uint8_t bsp_usb_get_hid_keyb_led_report(void)
{
    return (KeybReport.led_state);
}

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


void bsp_usb_handle_hid_keyb_clear_report_buffer(void)
{
    memset(&(KeybReport.report),0x00, sizeof(BSP_KEYB_REPORT_s));
}

void bsp_usb_handle_hid_mouse_clear_report_buffer(void)
{
    memset(&(MouseReport.report),0x00, sizeof(BSP_MOUSE_REPORT_s));
}

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
    config.intmask = USBINTMASK_SUSP | USBINTMASK_RESUME;
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


USB_HID_CTL_STA_t bsp_usb_hid_ctl_send(uint8_t *data, uint16_t len){

    if(!CtlReport.ready){
        return USB_HID_STA_NOT_READY;
    }

    if(CtlReport.sendBusy){
        return USB_HID_STA_BUSY;
    }

    if( data == NULL || len == 0 || len > MAX_REPORT_SIZE ){
        return USB_HID_STA_INVALID_PARAM;
    }

    USB_HID_CTL_STA_t error = USB_HID_STA_SUCCESS;

    USB_ERROR_TYPE_E status = bsp_usp_hid_ctl_tx_data_trigger(1, data, len);
    switch(status){
        case USB_ERROR_NONE:
            CtlReport.sendBusy = U_TRUE;
            break;
        case USB_ERROR_INVALID_INPUT:
            error = USB_HID_STA_INTERNAL_ERR;
            break;
        case USB_ERROR_INACTIVE_EP:
            error = USB_HID_STA_INACTIVE_EP;
            break;
        default:
            error = USB_HID_STA_UNKNOW_ERR;
            break;
    }

    return error;
}

void bsp_usb_hid_ctl_send_complete_callback_register(bsp_usb_hid_ctl_send_complete_cb_t cb){
    usb_hid_ctl_send_complete_callback = cb;
}

void bsp_usb_hid_ctl_recv_callback_register(bsp_usb_hid_ctl_recv_cb_t cb){
    usb_hid_ctl_recv_callback = cb;
}

