#include "stm32f10x.h"
#include "Encoder.h"

uint16_t EncoderNum;

void Encoder_GPIO_Config(void){
    GPIO_InitTypeDef GPIO_InitStucture;
    GPIO_InitStucture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStucture.GPIO_Pin = Encoder_PinA | Encoder_PinB;
    GPIO_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Encoder_GPIO,&GPIO_InitStucture);
}

void Encoder_ClockConfig(void){
    RCC_APB2PeriphClockCmd(Encoder_RCC | RCC_APB2Periph_AFIO,ENABLE);
}

void Encoder_AFIOConfig(void){
    GPIO_EXTILineConfig(Encoder_AFIOPort,Encoder_AFIOPinA);
    GPIO_EXTILineConfig(Encoder_AFIOPort,Encoder_AFIOPinB);
}

void Encoder_EXTIConfig(void){
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = Encoder_EXTILineA | Encoder_EXTILineB;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);
}

void Encoder_NVICConfig(void){
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);


}

void Encoder_Config(void){
    Encoder_ClockConfig();
    Encoder_GPIO_Config();
    Encoder_AFIOConfig();
    Encoder_EXTIConfig();
    Encoder_NVICConfig();
}


void EXTI0_IRQHandler(void){
    if(EXTI_GetITStatus(Encoder_EXTILineA)==SET)
	{
        if(GPIO_ReadInputDataBit(Encoder_GPIO,Encoder_PinA)==0)
        {
            if(GPIO_ReadInputDataBit(Encoder_GPIO,Encoder_PinB)==0)
		    {
			    EncoderNum++;
		    }
        }
	}
    EXTI_ClearITPendingBit(Encoder_EXTILineA);
}

void EXTI1_IRQHandler(void){
    if(EXTI_GetITStatus(Encoder_EXTILineB)==SET)
	{
        if(GPIO_ReadInputDataBit(Encoder_GPIO,Encoder_PinB)==0)
        {
            if(GPIO_ReadInputDataBit(Encoder_GPIO,Encoder_PinA)==0)
		    {
			    EncoderNum--;
		    }
        }
	}
    EXTI_ClearITPendingBit(Encoder_EXTILineB);
}

int16_t GetEncoderNum(void){
	 int16_t Temp;
	 Temp=EncoderNum;
	 EncoderNum=0;
	return Temp;
}
