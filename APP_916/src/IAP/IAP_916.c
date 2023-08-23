#include "IAP_916.h"
#include "platform_api.h"

// IAP INFO
const char iap_header[] = "INGCHIPS";
const char iap_chip[]   = "ING91683C_TB";
const char iap_poject[] = "HS_KB_SINGLE_COLOR_V01";
const char iap_HW[]     = "V1.0.1";
const char iap_SW[]     = "V2.1.0";


static void IAP_PrintInfo(void){
    platform_printf("\n--------- LOCAL IAP INFOs ----------\n");
    platform_printf("HEADER    : %s\n", iap_header);
    platform_printf("CHIP_INFO : %s\n", iap_chip);
    platform_printf("PROJECT   : %s\n", iap_poject);
    platform_printf("HW        : %s\n", iap_HW);
    platform_printf("SW        : %s\n", iap_SW);
    platform_printf("--------- LOCAL IAP INFOe ----------\n\n");
}

void IAP_Init(void){
    platform_printf("\n===>This is the APP code.\n");

    IAP_PrintInfo();

    
}

