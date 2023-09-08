#include "string.h"
#include "IAP_Application.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "btstack_util.h"
#include "IAP_Transport.h"
#include "eflash.h"
#include "rom_tools.h"
#include "IAP_UserDef.h"
#include "IAP_916.h"
#include "crc16.h"

#if USER_IAP_APP_ERROR_LOG_EN
#define IAP_APP_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_ERROR(...)      
#endif

#if USER_IAP_APP_DEBUG_LOG_EN
#define IAP_APP_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_DEBUG(...)      
#endif

// =================================================================================================

// 传输层接收APP命令使用此buffer存数据
// 传输层发送APP命令从此buffer取数据
static uint8_t appBuffer[IAP_APP_MAX_BUFFER_SIZE];

static IAP_APP_ctl_t cmdCtl = {
    // APP CMD control
    .rspCmd = 0x00,
    .buffer = (uint8_t *)&appBuffer[0],
    .size   = 0,
    .payload_size = 0,
    .payload = NULL,
};

// =================================================================================================

static void IAP_JumpToBoot_Delay_Timeout_Callback(void){
    AppJumpToBoot();
}

// =================================================================================================
uint8_t * IAP_GetAppBuffer(void){
    return (uint8_t *)&appBuffer[0];
}

// 只是初始化指针，并不会改变appBuffer的值，所以不影响应用数据解析。
static void IAP_APP_PreparePayloadDataStart(void){
    cmdCtl.payload  = IAP_GetAppBuffer() + IAP_APP_SEND_CMD_PAYLOAD_OFFSET;
    cmdCtl.payload_size = 0;
}

// 会改变应用buffer的值，所以要在处理完应用数据后再进行
static uint8_t IAP_APP_AddPayloadData(uint8_t *data, uint16_t len){
    if( len==0 && data==NULL ){
        IAP_APP_ERROR("[CMD] error: params.\n");
        return IAP_FAIL;
    }
    
    if( (len + cmdCtl.payload_size + IAP_APP_SEND_CMD_MIN_SIZE) > IAP_APP_MAX_BUFFER_SIZE ){
        IAP_APP_ERROR("[CMD] error: too much data to send.\n");
        return IAP_FAIL;
    }

    memcpy(cmdCtl.payload, data, len);
    cmdCtl.payload += len;
    cmdCtl.payload_size += len;

#if USER_IAP_APP_DEBUG_LOG_EN
    IAP_APP_DEBUG("ADD[%d]: ", len);
    printf_hexdump(data, len);
#endif

    return IAP_OK;
}

// send with payload. 
static void IAP_APP_SendACK(uint8_t error){

    IAP_APP_ACK_t * ACK = (IAP_APP_ACK_t *)cmdCtl.buffer;
    ACK->CMD      = IAP_CMD_ACK;
    ACK->errCode  = error;
    ACK->rspCmd   = cmdCtl.rspCmd;
    ACK->length   = cmdCtl.payload_size;
    cmdCtl.size   = cmdCtl.payload_size + IAP_APP_SEND_CMD_MIN_SIZE;
    uint16_t *CRC = (uint16_t *)&cmdCtl.buffer[cmdCtl.size-2];
    *CRC  = getCRC16((uint8_t *)cmdCtl.buffer, cmdCtl.size-2);

#if USER_IAP_APP_DEBUG_LOG_EN
    IAP_APP_DEBUG("APP SEND ACK[%d]: ", cmdCtl.size);
    printf_hexdump(cmdCtl.buffer, cmdCtl.size);
#endif

    IAP_Transport_send_multi_pack(cmdCtl.buffer, cmdCtl.size);
}

// calc crc.
static uint8_t IsAppCrcValid(uint8_t *data, uint16_t len){
    uint16_t *CRC = (uint16_t *)&data[len-2];
    if(*CRC != getCRC16(data, len-2)){
        IAP_APP_ERROR("[CMD] error: CRC: Calc[0x%04X], Recv[0x%04X]\n", getCRC16(data, len-2), *CRC);
        return IAP_INVALID;
    } else {
        IAP_APP_DEBUG("[CMD] CRC:[0x%04X]\n", *CRC);
    }
    return IAP_VALID;
}

// =================================================================================================


static IAP_APP_ErrCode_t IAP_CMD_SwitchToBoot_handler(uint8_t * payload, uint16_t length){

    IAP_APP_ErrCode_t errCode = IAP_APP_ERR_NONE;

    if(length != 2){
        IAP_APP_ERROR("[SB] error: LEN=%d\n", length);
        return IAP_APP_ERR_LENGTH;
    }

    uint16_t * delay_ms = (uint16_t *)payload;

    if ((*delay_ms) > IAP_CMD_SWITCH_BOOT_MAX_DELAY_MS){
        IAP_APP_ERROR("[SB] error: delay_ms=%d > %d\n", (*delay_ms), IAP_CMD_SWITCH_BOOT_MAX_DELAY_MS);
        return IAP_APP_ERR_PARAM;
    }

    platform_set_timer(IAP_JumpToBoot_Delay_Timeout_Callback, (uint32_t)((*delay_ms)*1000/625));

    return errCode;
}



// IAP_CMD_SWITCH_BOOT: 2F AA 00 00 80 07 00 C0 02 00 E8 03 A8 2B 2D
static IAP_APP_ErrCode_t IAP_APP_cmd_dispatch(uint8_t *data, uint16_t len){

    IAP_APP_ErrCode_t errCode = IAP_APP_ERR_NONE;

    // check param
    if(data == NULL || len < IAP_APP_RECV_CMD_MIN_SIZE){
        IAP_APP_ERROR("[CMD] error: param.\n");
        return IAP_APP_ERR_PARAM;
    }

    IAP_APP_cmd_t * APP_CMD = (IAP_APP_cmd_t *)data;

#if USER_IAP_APP_DEBUG_LOG_EN
    IAP_APP_DEBUG("CMD: 0x%02X\n", APP_CMD->CMD);
    IAP_APP_DEBUG("LEN: 0x%04X\n", APP_CMD->length);
    if(APP_CMD->length){
        IAP_APP_DEBUG("payload[%d]:\n", APP_CMD->length);
        printf_hexdump(APP_CMD->payload, APP_CMD->length);
    }
#endif

    // FOR RSP.
    cmdCtl.rspCmd = APP_CMD->CMD;

    // check CRC
    if (!IsAppCrcValid(data, len)){
        IAP_APP_ERROR("[CMD] error: CRC.\n");
        return IAP_APP_ERR_CRC;
    }

    // check payload LEN
    if (APP_CMD->length != (len-IAP_APP_RECV_CMD_MIN_SIZE)){
        IAP_APP_ERROR("[CMD] error: payload length: %d, %d\n", APP_CMD->length, (len-IAP_APP_RECV_CMD_MIN_SIZE));
        return IAP_APP_ERR_LENGTH;
    }

    // setup response payload data.
    IAP_APP_PreparePayloadDataStart();

    // Process cmd.
    switch(APP_CMD->CMD){

        case IAP_CMD_SWITCH_BOOT:{
            IAP_APP_DEBUG("CMD: SWTICH BOOT\n");
            errCode = IAP_CMD_SwitchToBoot_handler(APP_CMD->payload, APP_CMD->length);
        }break;

        default:
            IAP_APP_ERROR("[CMD] error: unsupported cmd: %d\n", APP_CMD->CMD);
            return IAP_APP_ERR_INVALID_CMD;
    }

    return errCode;
}

static void IAP_APP_recv_cmd_callback(uint8_t *recvData, uint16_t recvLen){
#if USER_IAP_APP_DEBUG_LOG_EN
    platform_printf("\n\n========>>>>>APP RECV[%d]: ", recvLen); printf_hexdump(recvData, recvLen);
#endif
    
    IAP_APP_ErrCode_t errCode = IAP_APP_cmd_dispatch(recvData, recvLen);
    if(IAP_APP_ERR_NONE != errCode){
        IAP_APP_ERROR("[CMD] error: 0x%02X\n", errCode);
    }

    IAP_APP_SendACK(errCode);
}


void IAP_Application_Init(void){
    IAP_Transport_recv_cmd_callback_register(IAP_APP_recv_cmd_callback);
}


