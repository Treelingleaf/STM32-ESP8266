#include "stm32f10x.h"
#include "SystemConfig.h"

void SystemConfig(void){
    OLED_Init();
    LED_Config();
    key_Init();
    W25Q64_InitConfig();
    Encoder_Config();
	Moter_Config();
}
