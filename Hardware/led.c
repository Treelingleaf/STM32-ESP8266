#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "W25Q64.h"
#include "led.h"

void LED_Config(void){
	
	GPIO_InitTypeDef GPIOA_Initstructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIOA_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_Initstructure.GPIO_Pin = LED1_PIN | LED2_PIN | LED3_PIN;
	GPIOA_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO,&GPIOA_Initstructure);
	GPIO_SetBits(LED_GPIO,LED1_PIN | LED2_PIN | LED3_PIN);
	
}

/**
  * @brief  将LED编号转换为LED的命令页码.
  * @note   此函数用于将LED的编号（LED1、LED2、LED3等）转换为相应的命令页码。
  * @param  LEDseq: 要转换的LED编号
  * @retval LED命令页码
  */
uint8_t LEDx_To_LEDxCMDPage(LEDx LEDseq){
	if(LEDseq == LED1){
		return LED1_CMDPage;
	}
	else if(LEDseq == LED2){
		return LED2_CMDPage;
	}
	else if(LEDseq == LED3){
		return LED3_CMDPage;
	}
	return 0;
}

uint16_t LEDx_To_LEDPin(LEDx LEDseq){
	if(LEDseq == LED1)
		return LED1_PIN;
	else if(LEDseq == LED2)
		return LED2_PIN;
	else if(LEDseq == LED3)
		return LED3_PIN;
	return 0;
}

/**
  * @brief  控制LED的状态，并记录状态到W25Q64闪存中.
  * @note   此函数用于控制LED的状态，并将状态记录到W25Q64闪存中，以便持久化保存。
  * @param  LEDseq: 要控制的LED编号（LED1或LED2）
  * @param  newState: 新的LED状态（ON或OFF）
  * @retval LEDxDate: 记录在W25Q64中的LED状态数据
  */
void LED_CMD(LEDx LEDseq,LEDState newState){
	if(newState == ON){
		GPIO_ResetBits(LED_GPIO,LEDseq);
	}
	else if(newState == OFF){
		GPIO_SetBits(LED_GPIO,LEDseq);
	}
}

/**
  * @brief  切换LED状态并记录状态到W25Q64闪存中.
  * @note   此函数用于切换LED的状态，并将状态记录到W25Q64闪存中，以便持久化保存。
  * @param  LEDseq: 要切换状态的LED编号（LED1或LED2）
  * @retval LEDxDate: 记录在W25Q64中的新LED状态数据
  */
void LEDx_Turn(LEDx LEDseq){
	if(GPIO_ReadInputDataBit(LED_GPIO,LEDx_To_LEDPin(LEDseq)) == ON){
		LED_CMD(LEDseq,OFF);
	}
	else if(GPIO_ReadInputDataBit(LED_GPIO,LEDx_To_LEDPin(LEDseq)) == OFF){
		LED_CMD(LEDseq,ON);
	}
}


/**
  * @brief  检测并控制LED状态.
  * @note   此函数用于检测指定LED（LED1、LED2、LED3等）的状态，如果LED状态为0x0F（ON），则将其关闭（OFF）；如果LED状态不是0x0F，将其打开（ON）。
  * @param  LEDseq: 要检测和控制的LED编号
  * @retval LED状态，0x0F表示LED已打开（ON），0x0E表示LED已关闭（OFF）
  */
uint8_t LED_Detection(LEDx LEDseq){
	uint8_t LEDxState = W25Q64_ReadCMD(LEDx_To_LEDxCMDPage(LEDseq));
	if(LEDxState == LED_ON){
		LED_CMD(LEDseq,ON);
		return ON;
	}
	else{
		LED_CMD(LEDseq,OFF);
		return OFF;
	}
	//return LEDxState;
}

uint8_t LED_ReadWithW25Q(LEDx LEDseq){
	if(GPIO_ReadInputDataBit(LED_GPIO,LEDx_To_LEDPin(LEDseq)) == OFF)
		return OFF;
	else
		return ON;
}


void LED_SynchronizeW25Q64(LEDx LEDseq){
	if(GPIO_ReadInputDataBit(LED_GPIO,LEDx_To_LEDPin(LEDseq)) == 0){
		W25Q64_EraseData(LEDx_To_LEDxCMDPage(LEDseq));
		W25Q64_WriteCMD(LEDx_To_LEDxCMDPage(LEDseq),LED_ON);
	}
	else if(GPIO_ReadInputDataBit(LED_GPIO,LEDx_To_LEDPin(LEDseq)) == 1){
		W25Q64_EraseData(LEDx_To_LEDxCMDPage(LEDseq));
		W25Q64_WriteCMD(LEDx_To_LEDxCMDPage(LEDseq),LED_OFF);
	}
}



