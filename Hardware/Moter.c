#include "stm32f10x.h"
#include "Moter.h"
#include "Encoder.h"
#include "W25Q64.h"


void Moter_PWMConfig(void){

	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = MOTER_PWM_PIN;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOTER_PWM_GPIOX, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(MOTER_PWM_CLOCKSOURCE);
	
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = MOTER_PWM_TIM_PERIOD - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = MOTER_PWM_TIM_PRESCALER - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(MOTER_PWM_TIMX, &TIM_TimeBaseInitStructure);

	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC4Init(MOTER_PWM_TIMX, &TIM_OCInitStructure);
	
	TIM_Cmd(MOTER_PWM_TIMX, ENABLE);

}

void Moter_PWM_SetCompare4(uint16_t Compare)
{
	TIM_SetCompare4(MOTER_PWM_TIMX, Compare);
}

void Moter_GPIOConfig(void){

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOTER_PINA | MOTER_PINB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOTER_GPIOX, &GPIO_InitStructure);
}

void Moter_Config(void){
    Moter_PWMConfig();
    Moter_GPIOConfig();
}

void Motor_SetSpeed(int8_t Speed)
{
	if (Speed >= 0)
	{
		GPIO_SetBits(MOTER_GPIOX, MOTER_PINA);
		GPIO_ResetBits(MOTER_GPIOX, MOTER_PINB);
		Moter_PWM_SetCompare4(Speed);
	}
	else
	{
		GPIO_ResetBits(MOTER_GPIOX, MOTER_PINA);
		GPIO_SetBits(MOTER_GPIOX, MOTER_PINB);
		Moter_PWM_SetCompare4(-Speed);
	}
}

void Moter_With_Encoder(int8_t *level){
	int8_t speed;
	*level += GetEncoderNum();
	speed = speed * *level;
	Motor_SetSpeed(speed);
	if(*level > 10 || *level < -10){
		*level =0;
	}
}

void Moter_SynchronizeW25Q64(int8_t level){
	W25Q64_EraseData(MOTER_CMDPage);
	W25Q64_WriteCMD(MOTER_CMDPage,level);
	Moter_With_Encoder(&level);
}

uint8_t Moter_Detection(void){
	int8_t speed;
	speed = W25Q64_ReadCMD(MOTER_CMDPage);
	return speed;
}

