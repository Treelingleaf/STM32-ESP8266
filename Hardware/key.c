#include "stm32f10x.h" // 根据您的实际情况包含正确的头文件
#include "Delay.h"
#include "key.h"


void key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = BUTTON1_PIN | BUTTON2_PIN | BUTTON3_PIN | BUTTON4_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(KEY_GPIOX, &GPIO_InitStructure);
}

// 通用的按键读取函数
// 返回值：0 表示没有按键按下，1 表示 BUTTON1_PIN 按下，2 表示 BUTTON2_PIN 按下
uint8_t key_Getnum(void) {
    if (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON1_PIN) == 0) {
        // 等待按键释放
        while (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON1_PIN) == 0);
        // 去抖动
        Delay_ms(20);
        return 1;
    }

    if (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON2_PIN) == 0) {
        // 等待按键释放
        while (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON2_PIN) == 0);
        // 去抖动
        Delay_ms(20);
        return 2;
    }

    if (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON3_PIN) == 0) {
        // 等待按键释放
        while (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON3_PIN) == 0);
        // 去抖动
        Delay_ms(20);
        return 3;
    }

    if (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON4_PIN) == 0) {
        // 等待按键释放
        while (GPIO_ReadInputDataBit(KEY_GPIOX, BUTTON4_PIN) == 0);
        // 去抖动
        Delay_ms(20);
        return 4;
    }

    return 0; // 没有按键按下
}
