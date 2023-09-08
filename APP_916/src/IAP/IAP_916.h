#ifndef _IAP_916_H_
#define _IAP_916_H_

#include <stdint.h>
#include "stdbool.h"
#include "IAP_FLASH_MAP.H"

#define BIT_0    (1<<0)
#define BIT_1    (1<<1)

// ERROR code.
#define IAP_OK          (0)
#define IAP_FAIL        (1)

#define IAP_INVALID     (0)
#define IAP_VALID       (1)


// version information.
#define IAP_VER_FLAG_UNINIT     0xFFFF
#define IAP_VER_FLAG_INITED     0x55AA

typedef struct 
{
	unsigned char len;
	unsigned char str[15];
}IAP_ChipIDTypedef;

typedef struct 
{
	unsigned char len;
	unsigned char str[23];
}IAP_ItemInfoTypedef;

// app version info.
typedef struct __attribute__((packed))
{
	IAP_ChipIDTypedef chip_id;
    IAP_ItemInfoTypedef item_info;
    uint8_t HW[6];
    uint8_t SW[6];
	
}IAP_AppVerInfoTypedef;

// version info.
typedef struct __attribute__((packed)){
    uint16_t ver_flag;
    uint16_t crcVal;
    IAP_AppVerInfoTypedef verInfo;
} IAP_FlashVerInfo_t;

void AppJumpToBoot(void);
extern void Uart_Send_Complete_Check(void);
void IAP_Init(void);

#endif


