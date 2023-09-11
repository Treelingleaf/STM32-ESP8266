#ifndef __DHT11_H__
#define __DHT11_H__

#define DHT11_DA_PIN        GPIO_Pin_8
#define DHT11_DA_GPIOX      GPIOA

int8_t DHT11ReadData(u8 *Humi, u8 *Temp);
u8 DHT11Init(void);
#endif
