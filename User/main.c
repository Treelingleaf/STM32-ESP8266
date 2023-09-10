#include "stm32f10x.h" // 包含 STM32F10x 系列的头文件
#include "SystemConfig.h"

uint8_t LED1State,LED3State;
int8_t Speed;

void StartUp(void){
	OLED_ShowString(1,1,"SystemIniting");
	Delay_ms(500);
	OLED_ShowString(1,1,"SystemIniting.");
	Delay_ms(500);
	OLED_ShowString(1,1,"SystemIniting..");
	Delay_ms(500);
	OLED_ShowString(1,1,"SystemIniting...");
	Delay_ms(500);
	LED1State = LED_Detection(LED1);
	LED3State = LED_Detection(LED3);
	Speed = Moter_Detection();
	Delay_ms(1000);
	OLED_Clear();	
}

void LED_Update(void){
	uint8_t KeyNum=key_Getnum();
		if(KeyNum == 1){
			LEDx_Turn(LED1);
		}
		if(KeyNum == 2){
			LEDx_Turn(LED3);
		}
		if(KeyNum == 3){
			LED_SynchronizeW25Q64(LED1);
			LED_SynchronizeW25Q64(LED3);
			LED1State = LED_Detection(LED1);
			LED3State = LED_Detection(LED3);
			Moter_SynchronizeW25Q64(Speed);
		}
		
		LED1State = LED_ReadWithW25Q(LED1);
		LED3State = LED_ReadWithW25Q(LED3);
		OLED_ShowString(1,1,"SYSTEM STATE:");
		if(LED1State == OFF)
			OLED_ShowString(2,1,"L1_ON ");
		else
			OLED_ShowString(2,1,"L1_OFF");
		if(LED3State == OFF)
			OLED_ShowString(2,9,"L3_ON ");
		else
			OLED_ShowString(2,9,"L3_OFF");
		OLED_ShowString(3,1,"MoterSpeed:");
		OLED_ShowSignedNum(3,12,Speed*10,3);

}



int main()
{
	SystemConfig();
	StartUp();
	while(1)
	{
		LED_Update();
		Speed += GetEncoderNum();
		Moter_With_Encoder(&Speed);
	}
}





