#ifndef __MOTER_H__
#define __MOTER_H__

#define MOTER_PWM_PIN               GPIO_Pin_3  //
#define MOTER_PWM_GPIOX             GPIOA       //
#define MOTER_PWM_CLOCKSOURCE       TIM2        //
#define MOTER_PWM_TIMX              TIM2
#define MOTER_PWM_TIM_PERIOD        100
#define MOTER_PWM_TIM_PRESCALER     72

#define MOTER_PINA                  GPIO_Pin_11
#define MOTER_PINB                  GPIO_Pin_12
#define MOTER_GPIOX                 GPIOA

void Moter_Config(void);
void Motor_SetSpeed(int8_t Speed);
void Moter_PWM_SetCompare4(uint16_t Compare);
void Moter_With_Encoder(int8_t *level);
void Moter_SynchronizeW25Q64(int8_t level);
uint8_t Moter_Detection(void);

#endif
