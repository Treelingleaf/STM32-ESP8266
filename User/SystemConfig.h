#ifndef __SYSTEMCONFIG_H__
#define __SYSTEMCONFIG_H__

#include "W25Q64.h"
#include "Delay.h"
#include "OLED.h"
#include "led.h"
#include "key.h"
#include "Encoder.h"
#include "Moter.h"
#include "DHT11.h"
#include "ESP8266.h"

// 在头文件中声明外部变量，这样可以在其他 C 文件中调用
extern u8 ESP_RX_STATE; // 外部声明接收状态变量


void SystemConfig(void);

#endif
