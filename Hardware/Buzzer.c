#include "stm32f10x.h"                  // Device header

void Buzzer_Init(void){
	GPIO_InitTypeDef GPIOA_Initstructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIOA_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_Initstructure.GPIO_Pin = GPIO_Pin_10;
	GPIOA_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIOA_Initstructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

void Buzzer_ON(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}

void Buzzer_OFF(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

void Buzzer_Return(void){
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_10) == 1)
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	else
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
}
