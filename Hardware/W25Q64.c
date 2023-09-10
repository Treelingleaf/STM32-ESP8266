#include "stm32f10x.h"
#include "W25Q64.h"

#include <string.h>
#include <stdlib.h>

#define MAX_ARRAY_SIZE 100

/**
  * @brief  初始化W25Q64闪存的GPIO配置.
  * @note   此函数用于配置W25Q64闪存的GPIO引脚，包括SCK、MISO、MOSI、和SS引脚。
  * @retval 无
  */
void W25Q64_GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    //SS,SCK,MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_SCK | W25Q64_PIN_MISO | W25Q64_PIN_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q64_GPIO,&GPIO_InitStructure);
    //MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_SS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q64_GPIO,&GPIO_InitStructure);
}

void W25Q64_Clock_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
}

void W25Q64_SPI_Init(void){
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(W25Q64_SPI,&SPI_InitStructure);
}

void SPI_State(SPI_SS_Level NewState){
    GPIO_WriteBit(W25Q64_GPIO,W25Q64_PIN_SS,(BitAction)NewState);
}

/**
  * @brief  配置W25Q64闪存SPI接收中断.
  * @note   此函数用于配置W25Q64闪存SPI接收中断，启用中断，并设置中断优先级。
  * @retval 无
  */
void W25Q64_ITConfig(void){
    NVIC_InitTypeDef NVIC_InitStructure;
    SPI_I2S_ITConfig(W25Q64_SPI,SPI_I2S_IT_RXNE,ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @illustrate:MCU Exchange Byte with W25Q64
 * @parameter:主机发给W25的数据（1字节）
 * @return:W25Q发给主机的数据
*/
uint8_t W25Q64_ExchangeByte(uint8_t DataByte){
    while (SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(W25Q64_SPI,DataByte);
    while (SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(W25Q64_SPI);
}

/**
  * @brief  读取W25Q64闪存的制造商ID和设备ID.
  * @note   此函数用于读取W25Q64闪存的制造商ID（MID）和设备ID（DID）。
  * @param  MID: 指向用于存储制造商ID的变量的指针
  * @param  DID: 指向用于存储设备ID的变量的指针
  * @retval 无
  */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID){
    SPI_State(Start);
    W25Q64_ExchangeByte(W25Q64_JEDEC_ID);
    *MID = W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE);
    *DID = W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE);
    *DID <<= 8;
    *DID |= W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE);
    SPI_State(Stop);
}


void W25Q64_WriteEnable(void)
{
	SPI_State(Start);
	W25Q64_ExchangeByte(W25Q64_WRITE_ENABLE);
	SPI_State(Stop);
}

/**
  * @brief  等待W25Q64闪存变为非忙状态.
  * @note   此函数用于等待W25Q64闪存变为非忙状态。在进行写操作之前，需要确保闪存不忙。
  * @retval 无
  */
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	SPI_State(Start);
	W25Q64_ExchangeByte(W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while ((W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
	SPI_State(Stop);
}


void W25Q64_EraseData(uint8_t Page){
    uint32_t Address = (0xF0 << 12) | (Page* 4096);
    W25Q64_WriteEnable();
	SPI_State(Start);
	W25Q64_ExchangeByte(W25Q64_SECTOR_ERASE_4KB);
	W25Q64_ExchangeByte(Address >> 16);
	W25Q64_ExchangeByte(Address >> 8);
	W25Q64_ExchangeByte(Address);
	SPI_State(Stop);
	W25Q64_WaitBusy();
}

void W25Q64_WriteArray(uint32_t RegAddress,uint8_t* Array){
    uint16_t count = 0;
    uint32_t ArrayLen =sizeof(Array)/sizeof(uint8_t);
    W25Q64_WriteEnable();
    SPI_State(Start);
    W25Q64_ExchangeByte(W25Q64_PAGE_PROGRAM);
    W25Q64_ExchangeByte(RegAddress >> 16);
    W25Q64_ExchangeByte(RegAddress >> 8);
    W25Q64_ExchangeByte(RegAddress);
    for(count = 0;count < ArrayLen;count++){
        W25Q64_ExchangeByte(Array[count]);
    }
    SPI_State(Stop);
    W25Q64_WaitBusy();
}

void W25Q64_WriteCMD(uint8_t Page,uint8_t Divice_Cmd){
    uint32_t Address = (0xF0 << 12) | (Page* 4096);
    W25Q64_WriteEnable();
    SPI_State(Start);
    W25Q64_ExchangeByte(W25Q64_PAGE_PROGRAM);
    W25Q64_ExchangeByte(Address >> 16);
    W25Q64_ExchangeByte(Address >> 8);
    W25Q64_ExchangeByte(Address);
    W25Q64_ExchangeByte(Divice_Cmd);
    SPI_State(Stop);
    W25Q64_WaitBusy();
}



void WriteStringToW25Q64(uint32_t RegAddress, const char* str) {
    // 计算字符串的长度，包括结尾的空字符 '\0'
    uint32_t str_length = strlen(str) + 1; // +1 是为了包括空字符 '\0'
    
    // 使用动态内存分配来创建一个字节数组，并分配足够的空间
    uint8_t *data = (uint8_t *)malloc(str_length);
    
    if (data == NULL) {
        // 处理内存分配失败的情况
        // 在实际应用中，应该进行错误处理
        return;
    }
    
    // 将字符串转换为字节数组
    memcpy(data, str, str_length);
    
    // 调用 W25Q64 写入函数，将字节数组写入 W25Q64
    W25Q64_WriteArray(RegAddress, data);
    
    // 释放动态分配的内存
    free(data);
}

uint8_t W25Q64_ReadCMD(uint8_t Page) {
    uint8_t Divice_Cmd;
    uint32_t Address = (0xF0 << 12) | (Page* 4096);
    SPI_State(Start);
    W25Q64_ExchangeByte(W25Q64_READ_DATA);
    W25Q64_ExchangeByte(Address >> 16);
    W25Q64_ExchangeByte(Address >> 8);
    W25Q64_ExchangeByte(Address);
    Divice_Cmd = W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE);
    SPI_State(Stop);
    return Divice_Cmd;
}

void W25Q64_ReadArray(uint32_t RegAddress, uint8_t* Array, uint32_t ArrayLen) {
    uint16_t count = 0;
    SPI_State(Start);
    W25Q64_ExchangeByte(W25Q64_READ_DATA);
    W25Q64_ExchangeByte(RegAddress >> 16);
    W25Q64_ExchangeByte(RegAddress >> 8);
    W25Q64_ExchangeByte(RegAddress);
    for(count = 0;count < ArrayLen;count++){
        Array[count] = W25Q64_ExchangeByte(W25Q64_DUMMY_BYTE);
    }
    SPI_State(Stop);
}

void ReadStringFromW25Q64(uint32_t RegAddress, char* str, uint32_t str_length) {
    // 动态分配内存来存储字节数组
    uint8_t *data = (uint8_t *)malloc(str_length);
    if (data == NULL) {
        // 处理内存分配失败的情况
        // 在实际应用中，应该进行错误处理
        return;
    }

    // 调用 W25Q64 读取函数，将数据读取到字节数组中
    W25Q64_ReadArray(RegAddress, data, str_length);
    
    // 将字节数组转换为字符串
    strncpy(str, (char*)data, str_length);
    
    // 确保字符串以空字符 '\0' 结尾
    str[str_length - 1] = '\0';

    // 释放动态分配的内存
    free(data);
}


void W25Q64_InitConfig(void){
    W25Q64_Clock_Init();
    W25Q64_GPIO_Init();
    W25Q64_SPI_Init();
    SPI_Cmd(W25Q64_SPI,ENABLE);
    SPI_State(Stop);
}

void SPI1_IRQHandler(void){

}
