#include "string.h"
#include "IAP_Application.h"
#include "platform_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_usb_hid_iap.h"
#include "btstack_util.h"
#include "IAP_Transport.h"
#include "eflash.h"
#include "rom_tools.h"

#if 1
#define IAP_APP_ERROR(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_ERROR(...)      
#endif

#if 1
#define IAP_APP_DEBUG(...)	platform_printf(__VA_ARGS__)
#else
#define IAP_APP_DEBUG(...)      
#endif

// 传输层接收APP命令使用此buffer存数据
// 传输层发送APP命令从此buffer取数据
static uint8_t appBuffer[IAP_APP_MAX_BUFFER_SIZE];

static IAP_APP_ctl_t cmdCtl = {
    .rspCmd = 0x00,
    .buffer = (uint8_t *)&appBuffer[0],
    .size   = 0,
    .upgrade_flag = 0,
};


// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

uint8_t * IAP_GetAppBuffer(void){
    return (uint8_t *)&appBuffer[0];
}

static uint16_t IAP_Get_CRC(uint8_t *buffer, uint16_t len){
    return crc(buffer, len);
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
        return 1;
    }
    
    if( (len + cmdCtl.payload_size + IAP_APP_SEND_CMD_MIN_SIZE) > IAP_APP_MAX_BUFFER_SIZE ){
        IAP_APP_ERROR("[CMD] error: too much data to send.\n");
        return 2;
    }

    memcpy(cmdCtl.payload, data, len);
    cmdCtl.payload += len;
    cmdCtl.payload_size += len;

#if 1
    IAP_APP_DEBUG("ADD[%d]: ", len);
    printf_hexdump(data, len);
#endif

    return 0;
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
    *CRC  = IAP_Get_CRC((uint8_t *)cmdCtl.buffer, cmdCtl.size-2);

#if 1
    IAP_APP_DEBUG("APP SEND ACK[%d]: ", cmdCtl.size);
    printf_hexdump(cmdCtl.buffer, cmdCtl.size);
#endif

    IAP_Transport_send_multi_pack(cmdCtl.buffer, cmdCtl.size);
}

// calc crc.
static uint8_t IsAppCrcValid(uint8_t *data, uint16_t len){
    uint16_t *CRC = (uint16_t *)&data[len-2];
    if(*CRC != IAP_Get_CRC(data, len-2)){
        IAP_APP_ERROR("[CMD] error: CRC: Calc[0x%04X], Recv[0x%04X]\n", IAP_Get_CRC(data, len-2), *CRC);
        return 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------
static void PrintStr(char *comment, char *str, uint8_t len){
    uint8_t i;
    IAP_APP_DEBUG("%s", comment);
    for(i=0; i<len; i++){
        IAP_APP_DEBUG("%c", str[i]);
    }
    IAP_APP_DEBUG("\n");
}

static void PrintHeaderInfo(IAP_HeaderTypedef * iapHeader){

    platform_printf("\n\n ------------------ HEADER INFO -------------------\n");
    PrintStr((char *)"upgradeFlag: ", (char *)&iapHeader->upgradeFlag, 8);

    IAP_APP_DEBUG("chip_id_len = %d\n", iapHeader->chip_id.len);
    PrintStr((char *)"chip_id: ", (char *)&iapHeader->chip_id.str, iapHeader->chip_id.len);

    IAP_APP_DEBUG("item_info_len = %d\n", iapHeader->item_info.len);
    PrintStr((char *)"item_info: ", (char *)&iapHeader->item_info.str, iapHeader->item_info.len);

    PrintStr((char *)"HW: ", (char *)&iapHeader->HW, 6);
    PrintStr((char *)"SW: ", (char *)&iapHeader->SW, 6);

    if (iapHeader->check.type == IAP_CHECK_TYPE_CRC){
        IAP_APP_DEBUG("check type = CRC\n");
        IAP_APP_DEBUG("check len = %d\n", iapHeader->check.len);
        IAP_APP_DEBUG("check data: 0x%04X\n", iapHeader->check.val.CRC); 
    } else {
        IAP_APP_DEBUG("check type = SUM\n");
        IAP_APP_DEBUG("check len = %d\n", iapHeader->check.len);
        IAP_APP_DEBUG("check data: 0x%04X\n", iapHeader->check.val.SUM);
    }

    IAP_APP_DEBUG("block size = 0x%04X\n", iapHeader->block.size);
    IAP_APP_DEBUG("block num  = 0x%04X\n", iapHeader->block.num);

    if(iapHeader->upgradeType == IAP_UPGRADE_TYPE_APP_ONLY)
        IAP_APP_DEBUG("upgradeType : APP Only.\n");
    else 
        IAP_APP_DEBUG("upgradeType : %d\n", iapHeader->upgradeType);

    IAP_APP_DEBUG("encrypt enable = %d\n", iapHeader->encrypt.enable);
    if (iapHeader->encrypt.enable){
        IAP_APP_DEBUG("encrypt type = %d\n", iapHeader->encrypt.type);
        IAP_APP_DEBUG("encrypt len = %d\n", iapHeader->encrypt.len);
        if(iapHeader->encrypt.type == IAP_ENCRYPT_TYPE_AES128){ //aes
            IAP_APP_DEBUG("encrypt type = AES128\n");
            IAP_APP_DEBUG("encrypt key: ");
            printf_hexdump(iapHeader->encrypt.key, iapHeader->encrypt.len/2);
            IAP_APP_DEBUG("encrypt iv: ");
            printf_hexdump(iapHeader->encrypt.iv, iapHeader->encrypt.len/2);
        } else {
            IAP_APP_DEBUG("encrypt type = XOR\n");
            IAP_APP_DEBUG("encrypt key: ");
            printf_hexdump(iapHeader->encrypt.key, iapHeader->encrypt.len);
        }
    }
}



// INFO BEGIN.
static const char bUpgradeFlag[] = "INGCHIPS";
static const char bChipID[]      = "ING91683C_TB";
static const char bItemStr[]     = "ING_USB_IAP_TEST";
static const char bHardware[]    = "V1.0.0";
static const char bSoftware[]    = "V2.1.3";
#define GET_STR_LEN(x)           (strlen(x))
// INFO END.

static uint8_t IAP_APP_VersionFormatValidCheck(char *str){
    if(str[0] != 'V')    return IAP_INVALID;
    if(str[1] < '0' || str[1] > '9')    return IAP_INVALID;
    if(str[2] != '.')    return IAP_INVALID;
    if(str[3] < '0' || str[3] > '9')    return IAP_INVALID;
    if(str[4] != '.')    return IAP_INVALID;
    if(str[5] < '0' || str[5] > '9')    return IAP_INVALID;
    return IAP_VALID;
}

static uint8_t IsCheckInfoValid(IAP_CheckInfoTypedef * check){

#if USER_CFG_IAP_CHECK_TYPE_SUPPORT_CRC_EN
    if(check->type == IAP_CHECK_TYPE_CRC && check->len == 2){
        return IAP_VALID;
    }
#endif

#if USER_CFG_IAP_CHECK_TYPE_SUPPORT_SUM_EN
    if(check->type == IAP_CHECK_TYPE_SUM && check->len == 2){
        return IAP_VALID;
    }
#endif

    return IAP_INVALID; //unsupported check type.
}



static uint8_t IsUpgradeTypeValid(uint8_t upgradeType){

#if USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_APP_ONLY_EN
    if( upgradeType == IAP_UPGRADE_TYPE_APP_ONLY ){
        return IAP_VALID;
    }
#endif

#if USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_APP_EN
    if( upgradeType == IAP_UPGRADE_TYPE_PLATFORM_APP ){
        return IAP_VALID;
    }
#endif

#if USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_BOOT_EN
    if( upgradeType == IAP_UPGRADE_TYPE_PLATFORM_BOOT ){
        return IAP_VALID;
    }
#endif

#if USER_CFG_IAP_UPGRADE_TYPE_SUPPORT_PLATFORM_BOOT_APP_EN
    if( upgradeType == IAP_UPGRADE_TYPE_PLAT_BOOT_APP ){
        return IAP_VALID;
    }
#endif

    return IAP_INVALID;
}

static uint32_t GetUpgradeAreaCodeSize(uint8_t upgradeType){
    
    if( upgradeType == IAP_UPGRADE_TYPE_APP_ONLY ){
        return APP_CODE_SIZE;
    }
    else if( upgradeType == IAP_UPGRADE_TYPE_PLATFORM_APP ){
        return (PLATFORM_CODE_SIZE + APP_CODE_SIZE);
    }
    else if( upgradeType == IAP_UPGRADE_TYPE_PLATFORM_BOOT ){
        return (PLATFORM_CODE_SIZE + BOOT_CODE_SIZE);
    }
    else if( upgradeType == IAP_UPGRADE_TYPE_PLAT_BOOT_APP ){
        return (PLATFORM_CODE_SIZE + BOOT_CODE_SIZE + APP_CODE_SIZE);
    }
    return 0xFFFFFFFF; // never come here.
}

static uint8_t IsBlockInfoValid(IAP_BlockInfoTypedef * block, uint8_t upgradeType){

    // block size check.
    if(block->size < IAP_MIN_BLOCK_SIZE || block->size > IAP_MAX_BLOCK_SIZE){
        IAP_APP_ERROR("[HEADER] error: invalid block_size! \n");
        return IAP_INVALID;
    }

    // Get code size.
    uint32_t upgradeCodeSize = GetUpgradeAreaCodeSize(upgradeType);

    // check upgrade code size.
    if( (block->size * block->num) > upgradeCodeSize){
        IAP_APP_ERROR("[HEADER] error: upgrade data too large! \n");
        return IAP_INVALID;
    }

    return IAP_VALID;
}

static uint8_t IsEncryptInfoValid(IAP_EncryptInfoTypedef * encrypt){

    // disable encrypt, return success.
    if (encrypt->enable == 0x00){
        return 1;
    }

#if USER_CFG_IAP_ENCRYPT_TYPE_SUPPORT_XOR_EN
    if( encrypt->type == IAP_ENCRYPT_TYPE_XOR ){
        if (encrypt->len != IAP_ENCRYPT_LEN_XOR){
            return IAP_INVALID;
        }
    }
#endif

#if USER_CFG_IAP_ENCRYPT_TYPE_SUPPORT_AES_EN
    else if( encrypt->type == IAP_ENCRYPT_TYPE_AES128 ){
        if (encrypt->len != IAP_ENCRYPT_LEN_AES128){
            return IAP_INVALID;
        }
    }
#endif

    else{
        return IAP_INVALID; //unsupported encrypt type.
    }

    return IAP_VALID; // success.
}



static IAP_APP_ErrCode_t IAP_Header_Check(IAP_HeaderTypedef * header){
    
    // Check upgradeFlag
    if (memcmp(header->upgradeFlag, bUpgradeFlag, GET_STR_LEN(bUpgradeFlag)) != 0){
        IAP_APP_ERROR("[HEADER] error: upgradeFlag\n");
        return IAP_APP_ERR_HEADER_UPGRADE_FLAG;
    }

    // Check CHIP_ID
    if ( (GET_STR_LEN(bChipID) != header->chip_id.len) || (memcmp(header->chip_id.str, bChipID, GET_STR_LEN(bChipID)) != 0) ){
        IAP_APP_ERROR("[HEADER] error: chipID\n");
        return IAP_APP_ERR_HEADER_CHIP_ID;
    }

    // Check item information
    if ( (GET_STR_LEN(bItemStr) != header->item_info.len) || (memcmp(header->item_info.str, bItemStr, GET_STR_LEN(bItemStr)) != 0) ){
        IAP_APP_ERROR("[HEADER] error: itemInfo\n");
        return IAP_APP_ERR_HEADER_ITEM_IINFO;
    }

    // Check hardware version
    if (!IAP_APP_VersionFormatValidCheck((char *)header->HW)){
        IAP_APP_ERROR("[HEADER] error: HW version.\n");
        return IAP_APP_ERR_HEADER_HARDWARE_VER;
    }

    // Check software version
    if (!IAP_APP_VersionFormatValidCheck((char *)header->SW)){
        IAP_APP_ERROR("[HEADER] error: SW version.\n");
        return IAP_APP_ERR_HEADER_SOFTWARE_VER;
    }

    // Check CHECK_TYPE
    if (!IsCheckInfoValid(&header->check)){
        IAP_APP_ERROR("[HEADER] error: CHECK INFO.\n");
        return IAP_APP_ERR_HEADER_CHECK_INFO;
    }
    
    // Check UpgradeType
    if (!IsUpgradeTypeValid(header->upgradeType)){
        IAP_APP_ERROR("[HEADER] error: upgrade type.\n");
        return IAP_APP_ERR_HEADER_UPGRADE_TYPE;
    }

    // Check block info
    if (!IsBlockInfoValid(&header->block, header->upgradeType)){
        IAP_APP_ERROR("[HEADER] error: BLOCK INFO.\n");
        return IAP_APP_ERR_HEADER_BLOCK_INFO;
    }

    // Check encrypt information
    if (!IsEncryptInfoValid(&header->encrypt)){
        IAP_APP_ERROR("[HEADER] error: encrypt.\n");
        return IAP_APP_ERR_HEADER_ENCRYPT;
    }

    return IAP_APP_ERR_NONE;
}

static IAP_APP_ErrCode_t IAP_CMD_Start_handler(uint8_t * payload, uint16_t length){

    IAP_APP_ErrCode_t errCode = IAP_APP_ERR_NONE;

    if (length != sizeof(IAP_HeaderTypedef)){
        IAP_APP_ERROR("[CMD] error: len: header[%d], Recv[%d]\n", sizeof(IAP_HeaderTypedef), length);
        return IAP_APP_ERR_LENGTH;
    }

    // get header struct.
    IAP_HeaderTypedef * header = (IAP_HeaderTypedef *)payload;

    // print header.
    PrintHeaderInfo(header);

    // check header.
    errCode = IAP_Header_Check(header);
    if(IAP_APP_ERR_NONE != errCode){
        IAP_APP_ERROR("[CMD] error: header check: 0x%02X\n", errCode);
        return errCode;
    }

    // store header information to flash.
    IAP_APP_DEBUG("TODO : Store header info to flash.\n");

    // start upgrade.
    cmdCtl.upgrade_flag = 1;

    return errCode;
}


// 3F AA 00 00 80 09 00 A0 04 00 11 22 33 44 AA BB 78
static IAP_APP_ErrCode_t IAP_APP_cmd_dispatch(uint8_t *data, uint16_t len){

    IAP_APP_ErrCode_t errCode = IAP_APP_ERR_NONE;

    // check param
    if(data == NULL || len < IAP_APP_RECV_CMD_MIN_SIZE){
        IAP_APP_ERROR("[CMD] error: param.\n");
        return IAP_APP_ERR_PARAM;
    }

    IAP_APP_cmd_t * APP_CMD = (IAP_APP_cmd_t *)data;

#if 1  //print.
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
        case IAP_CMD_START:{
            IAP_APP_DEBUG("CMD: START\n");
            errCode = IAP_CMD_Start_handler(APP_CMD->payload, APP_CMD->length);
        }break;
        
        case IAP_CMD_FLASH_WRITE:{
            IAP_APP_DEBUG("CMD: WRITE\n");
            if(!cmdCtl.upgrade_flag){
                IAP_APP_ERROR("[CMD] error: condition not satisfied.\n");
                errCode = IAP_APP_ERR_STATE_NOT_SATISFIED;
                break;
            }

        }break;

        case IAP_CMD_FLASH_READ:{
            IAP_APP_DEBUG("CMD: READ\n");
            // IAP_APP_AddPayloadData(DDD, LLL);
        }break;

        case IAP_CMD_REBOOT:{
            IAP_APP_DEBUG("CMD: REBOOT\n");
        }break;

        case IAP_CMD_SWITCH_APP:{
            IAP_APP_DEBUG("CMD: SWTICH APP\n");
        }break;

        default:
            IAP_APP_ERROR("[CMD] error: unsupported cmd: %d\n", APP_CMD->CMD);
            return IAP_APP_ERR_INVALID_CMD;
    }

    return errCode;
}

static void IAP_APP_recv_cmd_callback(uint8_t *recvData, uint16_t recvLen){
    platform_printf("\n\n========>>>>>APP RECV[%d]: ", recvLen); printf_hexdump(recvData, recvLen);
    
    IAP_APP_ErrCode_t errCode = IAP_APP_cmd_dispatch(recvData, recvLen);
    if(IAP_APP_ERR_NONE != errCode){
        IAP_APP_ERROR("[CMD] error: 0x%02X\n", errCode);
    }

    IAP_APP_SendACK(errCode);
}


void IAP_Application_Init(void){
    IAP_Transport_recv_cmd_callback_register(IAP_APP_recv_cmd_callback);
}


