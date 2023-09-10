#ifndef __ENCODER_H__
#define __ENCODER_H__

#define Encoder_PinA GPIO_Pin_0
#define Encoder_PinB GPIO_Pin_1
#define Encoder_GPIO GPIOB
#define Encoder_RCC RCC_APB2Periph_GPIOB

#define Encoder_AFIOPort GPIO_PortSourceGPIOB
#define Encoder_AFIOPinA GPIO_PinSource0
#define Encoder_AFIOPinB GPIO_PinSource1

#define Encoder_EXTILineA EXTI_Line0
#define Encoder_EXTILineB EXTI_Line1

void Encoder_Config(void);
int16_t GetEncoderNum(void);


#endif
