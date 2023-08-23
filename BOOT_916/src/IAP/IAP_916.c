#include "IAP_916.h"
#include "platform_api.h"
#include "eflash.h"
#include "FreeRTOS.h"
#include "task.h"

#define FLASH_BLOCK_SIZE    EFLASH_ERASABLE_SIZE // 916 = 4KB

// static uint8_t TempBuf[FLASH_BLOCK_SIZE];

extern void Uart_Send_Complete_Check(void);


/**
 * @brief JumpToApp
 * 
 * @param app_addr 
 */
void JumpToApp(const uint32_t app_addr){
    platform_printf("Jump to APP: 0x%X\n", app_addr);
    Uart_Send_Complete_Check();
    for(int i=20000;i>0;i--);
    platform_switch_app(app_addr);
}

/**
 * @brief Erase APP.
 * 
 * @param startAddress 
 * @param dataSize 
 */
void EraseAppCode(uint32_t startAddress, uint32_t dataSize){
    uint32_t block_num = (dataSize%FLASH_BLOCK_SIZE == 0)? (dataSize/FLASH_BLOCK_SIZE) : (dataSize/FLASH_BLOCK_SIZE + 1);
    for (uint32_t index=0; index<block_num; index++){
        erase_flash_sector(startAddress + FLASH_BLOCK_SIZE * index);
    }
}

/**
 * @brief Check APP.
 * 
 * @param startAddress 
 * @param dataSize 
 * @return int 
 */
int IsAppExist(uint32_t startAddress, uint32_t dataSize) {
    uint8_t *dataPtr = (uint8_t *)startAddress;

    for (uint32_t i = 0; i < dataSize; i++) {
        if (dataPtr[i] != 0xFF) {
            platform_printf("APP exist.\n");
            return 1; // Not all bytes are 0xFF, return 1
        }
    }

    platform_printf("APP NOT exist.\n");
    return 0; // All bytes are 0xFF, return 0
}

/**
 * @brief IAP_Task
 * 
 * @param pvParameters 
 */
static void IAP_Task(void *pvParameters){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        platform_printf("IAP_Task\n");
    }
}

/**
 * @brief IAP_Run
 * 
 */
static void IAP_Run(void){
    xTaskCreate((TaskFunction_t)IAP_Task,
                "IAP",
                configMINIMAL_STACK_SIZE,
                NULL,
                6,
                NULL);
}


/**
 * @brief IAP_Init
 * 
 */
void IAP_Init(void){
    platform_printf("\n===>This is the BOOT code.\n");

    // if (IsAppExist(APP_START_ADDR, 2*FLASH_BLOCK_SIZE)){
    //     platform_printf("Erase APP.\n");
    //     EraseAppCode(APP_START_ADDR, 2*FLASH_BLOCK_SIZE);
    //     // Check again.
    //     IsAppExist(APP_START_ADDR, 2*FLASH_BLOCK_SIZE);
    // }
    
    // IAP_PrintInfo();
    JumpToApp(APP_START_ADDR);

    // IAP_Run();

}

