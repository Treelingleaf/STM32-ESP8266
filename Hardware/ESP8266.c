#include "stm32f10x.h"
#include "SystemConfig.h"
#include "ESP8266.h"
#include <string.h>
#include "led.h"

void ESP_USART_Config(void){
    GPIO_InitTypeDef GPIO_InitSture;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    //Tx
    GPIO_InitSture.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitSture.GPIO_Pin = ESP_USART_TXPIN;
    GPIO_InitSture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_USART_GPIOX,&GPIO_InitSture);
    //Rx
    GPIO_InitSture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitSture.GPIO_Pin = ESP_USART_RXPIN;
    GPIO_InitSture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_USART_GPIOX,&GPIO_InitSture);  

    USART_InitStructure.USART_BaudRate = 115200;                      // 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;     // 发送和接收使能
    USART_InitStructure.USART_Parity = USART_Parity_No;                // 无奇偶校验
    USART_InitStructure.USART_StopBits = USART_StopBits_1;              // 1位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;        // 8位数据位
    USART_Init(ESP_USART, &USART_InitStructure);
    USART_ClearFlag(ESP_USART,USART_FLAG_CTS);
    USART_Cmd(ESP_USART, ENABLE);

    USART_ITConfig(ESP_USART, USART_IT_RXNE, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}

// 声明一个队列缓冲区，作为全局变量
ESP_BufTypeDef ESP_RX_BUF;                    // WiFi 模块串口接收数据缓冲区

// 接收缓冲区初始化 ESP 数据接收循环队列的初始化
void ESP_Rxbuf_Init(void)
{
    //int i;
    memset(ESP_RX_BUF.buf, 0, sizeof(ESP_RX_BUF.buf)); // 使用 memset() 函数将缓冲区清零，需要包含头文件 <string.h>

    // 或者可以使用循环将缓冲区清零
    // for (i = 0; i < ESP_BUF_SIZE; i++) {
    //     ESP_RX_BUF.buf[i] = 0;
    // }

    ESP_RX_BUF.front = 0;
    ESP_RX_BUF.length = 0;
    ESP_RX_BUF.rear = 0;
    
    ESP_RX_STATE = 0;          // 复位数据接收状态
}

// 串口接收中断函数，USART1串口接收中断
void USART1_IRQHandler(void)
{
    u8 rev_byte;
    LEDx_Turn(LED2);
    if (USART_GetITStatus(ESP_USART, USART_IT_RXNE) != RESET) // 检查接收缓冲区是否非空
    {
        rev_byte = USART_ReceiveData(ESP_USART); // 接收数据

        if (ESP_RX_BUF.length <= ESP_BUF_SIZE) // 如果数据没有溢出
        {
            ESP_RX_BUF.buf[ESP_RX_BUF.rear] = rev_byte; // 将数据存入缓冲区尾部
            ESP_RX_BUF.length++;                        // 缓冲区长度增加
            ESP_RX_BUF.rear = (ESP_RX_BUF.rear + 1) % ESP_BUF_SIZE; // 尾指针+1，防止溢出，实现循环队列
        }

        TIM_SetCounter(ESP_USART_TIMX, 0);     // 定时器清零
        TIM_Cmd(ESP_USART_TIMX, ENABLE);       // 使能定时器，启动定时器计时
    }
    USART_ClearITPendingBit(ESP_USART, USART_IT_RXNE); // 清除接收中断标志
}


// 读取ESP队列的数据，从ESP接收缓冲区中读取数据
u8 ESP_Read_Quene_Data(ESP_BufTypeDef* Rx_buf)
{
    u8 read_data_temp;

    if (Rx_buf->length == 0) // 如果队列中没有数据
    {
        read_data_temp = 0; // 返回0，表示没有数据可读
    }
    else
    {
        read_data_temp = Rx_buf->buf[Rx_buf->front]; // 读取头指针指向的数据
        Rx_buf->front = (Rx_buf->front + 1) % ESP_BUF_SIZE; // 更新头指针位置
        Rx_buf->length--; // 队列中的数据长度减少
    }

    return read_data_temp; // 返回读取的数据
}

/**
  * @brief 通过 USART 发送字符串.
  * @note 此函数用于通过指定的 USART 发送字符串，以单个字符的方式逐字符发送。
  * @param USARTx: 指定要使用的 USART 接口 (USART1, USART2, 等).
  * @param str: 要发送的字符串的指针.
  * @param timeout: 发送超时计数，如果为0表示无超时检查.
  * @retval 无
  */
void USART_Send_String(USART_TypeDef* USARTx, const char *str, uint32_t timeout) {
    // 遍历字符串中的每个字符
    while (*str) {
        // 将字符发送到 USART 数据寄存器
        USART_SendData(USARTx, (uint8_t)(*str));
        
        // 等待发送完成，如果设置了超时则进行相应处理
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {
            // 等待发送完成
            if (timeout > 0) {
                if (--timeout == 0) {
                    // 超时处理
                    break;
                }
            }
        }
        str++; // 移动到下一个字符
    }
}

// 宏定义返回值
#define ACK_SUCCESS 1
#define ACK_DEFEAT 0

// 检查接收到的字符串中是否包含指定的字符串
// Parameters:
// - string: 要检查的字符串
// Returns:
// - ACK_SUCCESS (1): 如果接收到的字符串中包含指定的字符串
// - ACK_DEFEAT (0): 如果接收到的字符串中不包含指定的字符串
u8 ESP_Check_Ack(char *string) {
    char *str_temp;               // 要检查的字符串的临时变量
    u8 data_temp;
    u8 rx_buff_temp[150] = {0};   // 临时存放接收到的数据的数组
    u8 pt_temp = 0;               // rx_buff_temp 数组的下标
    u8 length_temp = 0;           // 临时数组的当前长度

    str_temp = string;

    // 检查是否有接收到的数据
    if (ESP_RX_STATE > 0) {
        // 从缓冲区读取数据，直到遇到字符串结束符 '\0'
        do {
            if (length_temp < 150) {
                data_temp = ESP_Read_Quene_Data(&ESP_RX_BUF);  // 从缓冲区读取一个值到数组
                rx_buff_temp[pt_temp] = data_temp;
                pt_temp++;  // 下标向后移动一位
                length_temp++;
            } else {
                break;  // 缓冲区已满，不再接收数据，直接退出循环
            }
        } while (data_temp != '\0');

        ESP_RX_STATE--; // 读取完一个字符串，接收区计数减一
    }

    // 检查接收到的数据中是否包含指定的字符串
    if (strstr((const char *)rx_buff_temp, (const char *)str_temp) != NULL) {
        // 包含指定的字符串，返回成功
        return ACK_SUCCESS;
    } else {
        // 不包含指定的字符串，返回失败
        return ACK_DEFEAT;
    }
}

/**
 * 发送命令并等待回复
 * @param cmd 发送的命令
 * @param ack 希望得到的回复
 * @param waittime 等待的时长
 * @return 返回 ACK_SUCCESS 表示命令发送成功并收到期望的回复，返回 ACK_DEFEAT 表示发送失败或未收到期望的回复
 */
u8 ESP_Send_Cmd(char *cmd, char *ack, u16 waittime)
{
    USART_Send_String(ESP_USART, (u8 *)cmd ,0);  // 发送命令

    while (waittime--)
    {
        if (ESP_Check_Ack(ack) == ACK_SUCCESS)
        {
            return ACK_SUCCESS;  // 收到期望的回复，命令发送成功
        }
        Delay_ms(10);  // 每次等待10ms，所以waittime为20的时候，实际等待200ms
    }
    return ACK_DEFEAT;  // 超时或未收到期望的回复，命令发送失败
}

//AT+CIPSTART="TCP","192.168.137.1",8888

void ESP_ConnectTCPServer(void){
    if(ESP_Send_Cmd("AT+CIPSTART=\"TCP\",\"192.168.137.1\",8888\r\n","CONNECT",50)==ACK_SUCCESS)
    {
        OLED_Clear();
        OLED_ShowString(2,1,"TCP CONNECT OK!!");
        Delay_ms(1000);
    }
else{
        OLED_Clear();
        OLED_ShowString(2,1,"TCP CONNECT ERROR");
        Delay_ms(1000);
    }
}

void ESP_SetThroughMode(void){
    ESP_Send_Cmd("AT+CIPMODE=1\r\n","OK",20);
    ESP_Send_Cmd("AT+CIPSEND\r\n","OK",20);
}





