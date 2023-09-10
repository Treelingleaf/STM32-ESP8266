#ifndef __W25Q64_H__
#define __W25Q64_H__

//SPI1
#define W25Q64_PIN_SS     GPIO_Pin_4
#define W25Q64_PIN_SCK    GPIO_Pin_5
#define W25Q64_PIN_MISO   GPIO_Pin_6
#define W25Q64_PIN_MOSI   GPIO_Pin_7
#define W25Q64_GPIO       GPIOA
#define W25Q64_SPI        SPI1



//Key
#define KEY1_PIN          GPIO_Pin_1    //PB1
#define KEY2_PIN          GPIO_Pin_11   //PB11
#define KEY_GPIO          GPIOB

//W25Q64_CMD
#define W25Q64_WRITE_ENABLE							0x06
#define W25Q64_WRITE_DISABLE						0x04
#define W25Q64_READ_STATUS_REGISTER_1				0x05
#define W25Q64_READ_STATUS_REGISTER_2				0x35
#define W25Q64_WRITE_STATUS_REGISTER				0x01
#define W25Q64_PAGE_PROGRAM							0x02
#define W25Q64_QUAD_PAGE_PROGRAM					0x32
#define W25Q64_BLOCK_ERASE_64KB						0xD8
#define W25Q64_BLOCK_ERASE_32KB						0x52
#define W25Q64_SECTOR_ERASE_4KB						0x20
#define W25Q64_CHIP_ERASE							0xC7
#define W25Q64_ERASE_SUSPEND						0x75
#define W25Q64_ERASE_RESUME							0x7A
#define W25Q64_POWER_DOWN							0xB9
#define W25Q64_HIGH_PERFORMANCE_MODE				0xA3
#define W25Q64_CONTINUOUS_READ_MODE_RESET			0xFF
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB
#define W25Q64_MANUFACTURER_DEVICE_ID				0x90
#define W25Q64_READ_UNIQUE_ID						0x4B
#define W25Q64_JEDEC_ID								0x9F
#define W25Q64_READ_DATA							0x03
#define W25Q64_FAST_READ							0x0B
#define W25Q64_FAST_READ_DUAL_OUTPUT				0x3B
#define W25Q64_FAST_READ_DUAL_IO					0xBB
#define W25Q64_FAST_READ_QUAD_OUTPUT				0x6B
#define W25Q64_FAST_READ_QUAD_IO					0xEB
#define W25Q64_OCTAL_WORD_READ_QUAD_IO				0xE3
#define W25Q64_DUMMY_BYTE							0xFF


#define LED1_CMDPage    1
#define LED2_CMDPage    2
#define LED3_CMDPage    3
#define MOTER_CMDPage    4

#define LED_ON 0x0F
#define LED_OFF 0x0E

//SPI_SS
typedef enum SPI_SS_Level{
    Start=0,
    Stop=1
}SPI_SS_Level;



void W25Q64_InitConfig(void);
void ReadStringFromW25Q64(uint32_t RegAddress, char* str, uint32_t str_length);
void W25Q64_ReadArray(uint32_t RegAddress, uint8_t* Array, uint32_t ArrayLen);
void WriteStringToW25Q64(uint32_t RegAddress, const char* str);
void W25Q64_WriteArray(uint32_t RegAddress,uint8_t* Array);
void W25Q64_EraseData(uint8_t Page);
void W25Q64_WriteCMD(uint8_t Page,uint8_t Divice_Cmd);
uint8_t W25Q64_ReadCMD(uint8_t Page);


#endif
