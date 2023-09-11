#include "stm32f10x.h"
#include "DHT11.h"
#include "Delay.h"

void DHT11_SetDAState(u8 Newstate) {
    GPIO_WriteBit(DHT11_DA_GPIOX, DHT11_DA_PIN, (BitAction)Newstate);
}

u8 DHT11_GetDAState(void) {
    u8 DAstate = GPIO_ReadInputDataBit(DHT11_DA_GPIOX, DHT11_DA_PIN);
    return DAstate;
}

/**
  * @brief  复位DHT11并检查其响应.
  * @note   此函数用于复位DHT11传感器，并检查其是否正确响应。
  * @retval 1表示成功复位和响应，0表示失败
  */
uint8_t DHT11RstAndCheck(void) {
    uint8_t timer = 0;
    __set_PRIMASK(1);          // 关闭总中断
    DHT11_SetDAState(0);       // 输出低电平
    Delay_ms(20);              // 拉低至少18ms
    DHT11_SetDAState(1);       // 输出高电平
    Delay_us(30);              // 拉高20~40us
    while (DHT11_GetDAState() == 0) {
        timer++;               // 总线拉低时计数
        Delay_us(1);
    }
    if (timer > 100 || timer < 20) {
        __set_PRIMASK(0);      // 开启总中断
        return 0;
    }
    timer = 0;
    while (DHT11_GetDAState() == 1) {
        timer++;               // 总线拉高时计数
        Delay_us(1);
    }
    __set_PRIMASK(0);          // 开启总中断
    if (timer > 100 || timer < 20) {
        return 0;
    }
    return 1;
}


/**
  * @brief  从DHT11传感器中读取一个字节的数据.
  * @note   此函数从DHT11传感器中读取一个字节的数据。
  * @retval 读取的字节数据
  */
u8 DHT11ReadByte(void) {
    u8 i;
    u8 byt = 0;
    __set_PRIMASK(1);      // 关闭总中断
    for (i = 0; i < 8; i++) {
        while (DHT11_GetDAState() == 1);
        while (DHT11_GetDAState() == 0);
        Delay_us(40);
        byt <<= 1;
        if (DHT11_GetDAState() == 1) {
            byt |= 0x01;
        }
    }
    __set_PRIMASK(0);      // 开启总中断
    return byt;
}


/**
  * @brief  从DHT11传感器中读取湿度和温度数据.
  * @note   此函数从DHT11传感器中读取湿度和温度数据，并将结果存储在提供的指针中。
  * @param  Humi: 存储湿度数据的指针
  * @param  Temp: 存储温度数据的指针
  * @retval 返回值表示操作状态：1表示成功读取数据，-1表示读取失败
  */
int8_t DHT11ReadData(u8 *Humi, u8 *Temp) {
    s8 sta = 0;
    u8 i;
    u8 buf[5];
    
    // 重置DHT11并检查响应
    if (DHT11RstAndCheck()) {
        // 读取5个字节的数据
        for (i = 0; i < 5; i++) {
            buf[i] = DHT11ReadByte();
        }
        
        // 检查数据的校验和
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4]) {
            *Humi = buf[0]; // 湿度数据
            *Temp = buf[2]; // 温度数据
        }
        sta = 1; // 读取成功
    } else {
        *Humi = 0xFF; // 读取失败时，设置湿度和温度为0xFF表示无效数据
        *Temp = 0xFF;
        sta = -1; // 读取失败
    }
    return sta; // 返回读取状态
}


/**
  * @brief  初始化DHT11传感器.
  * @note   此函数用于初始化DHT11传感器，并检查传感器是否正常响应。
  * @retval 返回值表示初始化状态：1表示初始化成功，0表示初始化失败
  */
u8 DHT11Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 将DHT11引脚置为高电平
    GPIO_SetBits(DHT11_DA_GPIOX, DHT11_DA_PIN);
    
    // 配置DHT11引脚为开漏输出模式
    GPIO_InitStructure.GPIO_Pin = DHT11_DA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_DA_GPIOX, &GPIO_InitStructure);
    
    // 检查DHT11传感器是否正常响应
    return DHT11RstAndCheck();
}

