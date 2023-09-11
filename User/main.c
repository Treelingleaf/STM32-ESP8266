#include "stm32f10x.h" // 包含 STM32F10x 系列的头文件
#include "SystemConfig.h"

uint8_t LED1State,LED3State;
int8_t Speed;
uint8_t Humi,Temp;
char* str;

/**
  * @brief  系统启动初始化函数.
  * @note   此函数用于系统启动时的初始化操作，包括OLED显示、LED状态检测、电机速度检测等。
  * @retval 无
  */
void StartUp(void) {
    // 清除OLED显示
    OLED_Clear();
    
    // 显示系统初始化信息
    OLED_ShowString(1, 1, "SystemIniting");
    Delay_ms(500);
    OLED_ShowString(1, 1, "SystemIniting.");
    Delay_ms(500);
    OLED_ShowString(1, 1, "SystemIniting..");
    Delay_ms(500);
    OLED_ShowString(1, 1, "SystemIniting...");
    Delay_ms(500);
    if(DHT11Init() == 0){
        OLED_ShowString(2,1,"DHT11 Error");
        Delay_ms(500);
    }
    else{
        OLED_ShowString(2,1,"DHT11 Success");
        Delay_ms(500);
    }
    // 检测LED1和LED3的状态
    LED1State = LED_Detection(LED1);
    LED3State = LED_Detection(LED3);
    
    // 检测电机速度
    Speed = Moter_Detection();
    
    // 延时1秒
    Delay_ms(1000);

    ESP_ConnectTCPServer();
    ESP_SetThroughMode();
    
    // 清除OLED显示
    OLED_Clear();
}


/**
  * @brief  系统状态更新函数.
  * @note   此函数用于更新系统的状态信息，包括检测按键、同步LED状态和电机速度、显示系统状态信息等。
  * @retval 无
  */
void System_Update(void) {
    // 检测按键
    uint8_t KeyNum = key_Getnum();
    
    // 根据按键号执行不同操作
    if (KeyNum == 1) {
        LEDx_Turn(LED1);
        USART_Send_String(ESP_USART,"Hello World!",0);
    }
    if (KeyNum == 2) {
        LEDx_Turn(LED3);
        USART_Send_String(ESP_USART,str,0);
    }
    if (KeyNum == 3) {
        // 同步LED状态和电机速度到W25Q64
        LED_SynchronizeW25Q64(LED1);
        LED_SynchronizeW25Q64(LED3);
        LED1State = LED_Detection(LED1);
        LED3State = LED_Detection(LED3);
        Moter_SynchronizeW25Q64(Speed);
    }
    
    // 从W25Q64中读取LED状态
    LED1State = LED_ReadWithW25Q(LED1);
    LED3State = LED_ReadWithW25Q(LED3);
    
    // 在OLED上显示系统状态信息
    OLED_ShowString(1, 1, "SYSTEM STATE:");
    if (LED1State == OFF) {
        OLED_ShowString(2, 1, "L1_OFF");
    } else {
        OLED_ShowString(2, 1, "L1_ON ");
    }
    if (LED3State == OFF) {
        OLED_ShowString(2, 9, "L3_OFF");
    } else {
        OLED_ShowString(2, 9, "L3_ON ");
    }
    OLED_ShowString(3, 1, "MoterSpeed:");
    OLED_ShowSignedNum(3, 12, Speed * 10, 3);
    
    // 读取DHT11传感器数据并在OLED上显示
    if (DHT11ReadData(&Humi, &Temp) == -1) {
        OLED_ShowString(4, 1, "Data error");
    } else {
        OLED_ShowNum(4, 1, Humi, 3);
        OLED_ShowNum(4, 5, Temp, 3);
    }
}



int main()
{
	SystemConfig();
	StartUp();
    WriteStringToW25Q64(ESP_TCPPage,"AT+CIPSTART=\"TCP\",\"192.168.137.1\",8888\r\n");
    ReadStringFromW25Q64(ESP_TCPPage,str,1024);
    if(key_Getnum()==4){
        USART_Send_String(ESP_USART,"Hello World!",0);
        LEDx_Turn(LED2);
    }
	while(1)
	{
		System_Update();
		Speed += GetEncoderNum();
		Moter_With_Encoder(&Speed);
        
	}
}





