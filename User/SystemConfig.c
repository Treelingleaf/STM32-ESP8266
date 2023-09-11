#include "stm32f10x.h"
#include "SystemConfig.h"

// 定时器 TIM3 中断优先级配置
void TIM3_NVIC_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 设置中断优先级分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // 配置定时器3中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 设置子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure);
}

// 定时器 TIM3 初始化，10ms 定时器 TIME3 初始化，10ms
void TIM3_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // TIM3 初始化结构体
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 使能 TIM3 时钟
    TIM_DeInit(TIM3); // 复位 TIM3 寄存器

    TIM_TimeBaseStructure.TIM_Period = (2000 - 1); // 自动重装载寄存器的值
    TIM_TimeBaseStructure.TIM_Prescaler = (3600 - 1); // 时钟预分频数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频因子
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数模式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 初始化 TIM3

    TIM3_NVIC_config(); // 配置 TIM3 中断优先级

    TIM_ClearFlag(TIM3, TIM_FLAG_Update); // 清除更新标志位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 使能 TIM3 更新中断
}


void SystemConfig(void){
    OLED_Init();
    LED_Config();
    key_Init();
    W25Q64_InitConfig();
    Encoder_Config();
	Moter_Config();   
    TIM3_init();
    ESP_USART_Config();
}




// 定义一个全局变量用来指示是否接收到数据
// 超过10ms没有收到数据认为接收结束，则 ESP_RX_STATE 增加1。即定时器中断就加1
u8 ESP_RX_STATE = 0; // 初始化已接收数据状态为0





// TIM3 中断处理函数 TIM3 中断处理函数
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 如果触发了更新中断
    {
        ESP_RX_STATE++; // 超过 10ms，数据接收完毕，计数+1，超过10ms未收到数据，表示数据接收完成
    }
    TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update); // 清除更新中断标志位
    TIM_Cmd(TIM3, DISABLE); // 关闭 TIM3 定时器
}
