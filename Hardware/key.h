#ifndef __KEY_H__
#define __KEY_H__

#define KEY_GPIOX       GPIOB
#define BUTTON1_PIN     GPIO_Pin_6
#define BUTTON2_PIN     GPIO_Pin_7
#define BUTTON3_PIN     GPIO_Pin_5


void key_Init(void);
uint8_t key_Getnum(void);
#endif

