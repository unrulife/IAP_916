#include "string.h"
#include "IAP_Transport.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_usb_hid_iap.h"
#include "btstack_util.h"


#if 1
#define TRANSPORT_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define TRANSPORT_ERROR(...)      
#endif

#if 0
#define TRANSPORT_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define TRANSPORT_DEBUG(...)      
#endif


IAP_Transport_Ctl_t TransportCtl;
IAP_Transport_Data_t TransportData;
static iap_transport_recv_cmd_cb_t     iap_transport_recv_cmd_callback = NULL;



static uint8_t IAP_TransportGetBCC(uint8_t *data, uint16_t len){
    uint8_t BCC = data[0];
    for(uint16_t i=1; i<len; i++){
        BCC ^= data[i];
    }
    return BCC;
}

static uint8_t IAP_TransportCheckBCC(uint8_t *data, uint16_t len){
    uint8_t BCC = IAP_TransportGetBCC(data, len-1);
    if(BCC != data[len-1]){
        TRANSPORT_ERROR("[Transport] error: BCC calc=[0x%02X], recv=[0x%02X]\n", BCC, data[len-1]);
        return 1;
    }
    return 0;
}

static void IAP_TransportClearData(void){
    memset((uint8_t *)&TransportData.data, 0, sizeof(TransportData)); //
    TransportData.size = 0;
}

static uint8_t IAP_TransportAttachData(uint8_t *data, uint16_t len){
    if ((TransportData.size+len) > IAP_TRANSPORT_MAX_RECV_DATA_SIZE)
        return 1;
    memcpy((uint8_t *)&TransportData.data[TransportData.size], data, len);
    TransportData.size += len;
    return 0;
}

static void IAP_TransportGotoIdle(void){
    memset(&TransportCtl, 0, sizeof(TransportCtl));
    TransportCtl.state = IAP_TRANSPORT_STA_IDLE;
    IAP_TransportClearData();
}

static IAP_TransportErr_t IAP_Transport_Dispatch(uint8_t *data, uint16_t len){

    IAP_Transport_Recv_t * transport = (IAP_Transport_Recv_t *)data;

#if 0
    TRANSPORT_DEBUG("header: 0x%02x\n", transport->header);
    TRANSPORT_DEBUG("ctl_1: 0x%02x\n", transport->ctl_1);
    TRANSPORT_DEBUG("pack_ctl: 0x%04x\n", transport->pack_ctl);
    TRANSPORT_DEBUG("length: 0x%04x\n", transport->payload_length);
    if(transport->payload_length){
        TRANSPORT_DEBUG("payload[%d]:\n", transport->payload_length);
        printf_hexdump(transport->payload, transport->payload_length);
    }
#endif

    // check len.
    if(len < IAP_TRANSPORT_MIN_LEN){
        TRANSPORT_ERROR("[Transport] error: len too small\n");
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_LENGTH;
    }

    // check header.
    if(transport->header != IAP_TRANSPORT_HEADER){
        TRANSPORT_ERROR("[Transport] error: header\n");
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_HEADER;
    }

    // check direction.
    if(!IS_DIRECTION_DOWN(transport->ctl_1)){
        TRANSPORT_ERROR("[Transport] error: direction\n");
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_DIRECTION;
    }

    // check pack type.
    if(!IS_PACK_TYPE_DATA(transport->ctl_1)){
        TRANSPORT_ERROR("[Transport] error: pack type\n");
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_PACK_TYPE;
    }

    // check data length
    if(transport->payload_length > (len - IAP_TRANSPORT_MIN_LEN)){
        TRANSPORT_ERROR("[Transport] error: payload length: %d > %d\n", transport->payload_length, (len - IAP_TRANSPORT_MIN_LEN));
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_PACK_TYPE;
    }

    // check BCC. [ctl_1 ~ BCC] [without filled bytes.]
    if(IAP_TransportCheckBCC(&transport->ctl_1, transport->payload_length + IAP_TRANSPORT_MIN_LEN - 1)){
        TRANSPORT_ERROR("[Transport] error: BCC\n");
        IAP_TransportGotoIdle();
        return IAP_TRANSPORT_ERR_BCC;
    }
    
    // recv data.
    switch(TransportCtl.state){
        case IAP_TRANSPORT_STA_IDLE:{
            // first pack handle.
            if(IS_FIRST_PACK(transport->pack_ctl)){

                // store data.
                if(IAP_TransportAttachData(transport->payload, transport->payload_length)){
                    TRANSPORT_ERROR("[Transport] error: buffer overflow1\n");
                    IAP_TransportGotoIdle();
                    return IAP_TRANSPORT_ERR_BUFFER_OVERFLOW;
                }

                // switch state.
                if (IS_LAST_PACK(transport->pack_ctl)){
                    if(iap_transport_recv_cmd_callback)
                        iap_transport_recv_cmd_callback(TransportData.data, TransportData.size);
                    IAP_TransportGotoIdle();
                } else {
                    // need next pack.
                    TransportCtl.total_pack_num = GET_PACK_NUM(transport->pack_ctl);
                    TransportCtl.next_pack_num = TransportCtl.total_pack_num-1;
                    TransportCtl.state = IAP_TRANSPORT_STA_RECVING;
                }
            } else {
                TRANSPORT_ERROR("[Transport] error: not first pack\n");
                IAP_TransportGotoIdle();
                return IAP_TRANSPORT_ERR_PACK_NUM;
            }
        }
        break;
        case IAP_TRANSPORT_STA_RECVING:{

            // first pack ?
            if(IS_FIRST_PACK(transport->pack_ctl)){
                TRANSPORT_ERROR("[Transport] error: repeat first pack.\n");
                IAP_TransportGotoIdle();
                return IAP_TRANSPORT_ERR_PACK_NUM;
            }

            // check pack number.
            if (TransportCtl.next_pack_num != GET_PACK_NUM(transport->pack_ctl)){
                TRANSPORT_ERROR("[Transport] pack num error: recv(%d), need(%d)\n", GET_PACK_NUM(transport->pack_ctl), TransportCtl.next_pack_num);
                IAP_TransportGotoIdle();
                return IAP_TRANSPORT_ERR_PACK_NUM;
            }

            // store data.
            if(IAP_TransportAttachData(transport->payload, transport->payload_length)){
                TRANSPORT_ERROR("[Transport] error: buffer overflow2\n");
                IAP_TransportGotoIdle();
                return IAP_TRANSPORT_ERR_BUFFER_OVERFLOW;
            }

            // swtich state.
            if(IS_LAST_PACK(transport->pack_ctl)){
                if(iap_transport_recv_cmd_callback)
                    iap_transport_recv_cmd_callback(TransportData.data, TransportData.size);
                IAP_TransportGotoIdle();
            } else {
                if(TransportCtl.next_pack_num)
                    TransportCtl.next_pack_num--;
            }
        }
        break;
    }
    return IAP_TRANSPORT_ERR_NONE;
}

// AA C0 00 80 01 00 ERR BCC
static USB_HID_IAP_STA_t IAP_Transport_Send_ACK(uint8_t err){
    IAP_Transport_ACK_t ACK;
    ACK.header   = IAP_TRANSPORT_HEADER;
    ACK.ctl_1    = 0xC0; //up, ack
    ACK.pack_ctl = 0x8000; //first flag=1, num=0
    ACK.length   = 0x0001; //len=1
    ACK.errCode  = err;
    ACK.BCC      = IAP_TransportGetBCC(&ACK.ctl_1, sizeof(IAP_Transport_ACK_t)-2);
    return bsp_usb_hid_iap_send((uint8_t *)&ACK, sizeof(IAP_Transport_ACK_t));
}

// 此函数不支持自动分包发送。 
uint8_t IAP_Transport_send_single_pack(uint8_t *data, uint16_t len){
    if(len > IAP_TRANSPORT_MAX_SEND_SINGLE_PAYLOAD){
        return (uint8_t)USB_HID_STA_SIZE_TOO_LARGE;
    }

    IAP_Transport_Send_t * sendRSP = (IAP_Transport_Send_t * )&TransportData.data[0];
    sendRSP->header   = IAP_TRANSPORT_HEADER;
    sendRSP->ctl_1    = 0x80;   //up, data
    sendRSP->pack_ctl = 0x8000; //first flag=1, num=0
    sendRSP->length   = len; //len=1
    memcpy(sendRSP->payload, data, len);
    sendRSP->payload[len] = IAP_TransportGetBCC(&sendRSP->ctl_1, 1+2+2+len);
    TransportData.size = len+IAP_TRANSPORT_MIN_LEN;

#if 0
    platform_printf("send[%d]: ", TransportData.size);
    printf_hexdump(TransportData.data, TransportData.size);
#endif

    return (uint8_t)bsp_usb_hid_iap_send(TransportData.data, TransportData.size);
}


void IAP_Transport_recv_cmd_callback_register(iap_transport_recv_cmd_cb_t cb){
    iap_transport_recv_cmd_callback = cb;
}


void bsp_usb_hid_iap_recv_callback(uint8_t *data, uint16_t len){
    
#if 0
    platform_printf("RECV[%d]: ", len);
    printf_hexdump(data, len);
#endif

    // platform_printf("SEND(%d) ...\n", bsp_usb_hid_iap_send((uint8_t *)"\x88\x99\x33\x44", 4) );

    IAP_TransportErr_t errCode = IAP_Transport_Dispatch(data, len);
    if(errCode != IAP_TRANSPORT_ERR_NONE){
        IAP_Transport_Send_ACK(errCode);
    }
}


void bsp_usb_hid_iap_send_complete_callback(void){
    TRANSPORT_DEBUG("Send complete.\n");
}


void IAP_Transport_Init(void){
    IAP_TransportGotoIdle();
    IAP_TransportClearData();
    bsp_usb_hid_iap_recv_callback_register(bsp_usb_hid_iap_recv_callback);
    bsp_usb_hid_iap_send_complete_callback_register(bsp_usb_hid_iap_send_complete_callback);
}

