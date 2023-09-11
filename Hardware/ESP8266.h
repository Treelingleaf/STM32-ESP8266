#ifndef __ESP8266_H__
#define __ESP8266_H__

//USART1
//Tx -- PA9       Rx -- PA10
#define ESP_USART_TXPIN         GPIO_Pin_9
#define ESP_USART_RXPIN         GPIO_Pin_10
#define ESP_USART_GPIOX         GPIOA
#define ESP_USART               USART1
#define ESP_USART_TIMX          TIM3
// 在头文件中声明外部变量，这样可以在其他 C 文件中调用

// 宏定义缓冲队列最大存储上限
#define ESP_BUF_SIZE   1024     // ESP 缓冲区最大容量

// 循环队列结构体
typedef struct 
{
    unsigned char buf[ESP_BUF_SIZE];          // 缓冲区数组
    unsigned short int length;                // 缓冲区当前长度
    unsigned short int front;                 // 队列头索引
    unsigned short int rear;                  // 队列尾索引
} ESP_BufTypeDef;                             // 定义 ESP 数据接收循环队列结构体

extern ESP_BufTypeDef ESP_RX_BUF;            // WiFi 模块串口接收数据缓冲区


void ESP_USART_Config(void);
void USART_Send_String(USART_TypeDef* USARTx, const char *str, uint32_t timeout);
u8 ESP_Read_Quene_Data(ESP_BufTypeDef* Rx_buf);
u8 ESP_Send_Cmd(char *cmd, char *ack, u16 waittime);
void ESP_SetThroughMode(void);
void ESP_ConnectTCPServer(void);

#endif
