
#ifndef __LED_H__
#define __LED_H__

//LED
#define LED1_PIN          GPIO_Pin_0
#define LED2_PIN          GPIO_Pin_1
#define LED3_PIN          GPIO_Pin_2
#define LED_GPIO          GPIOA

typedef enum LEDx{
    LED1 = LED1_PIN,
    LED2 = LED2_PIN,
    LED3 = LED3_PIN
}LEDx;

typedef enum LEDState{
    ON = 0,
    OFF = 1
}LEDState;

void LED_CMD(LEDx LEDseq,LEDState newState);
void LEDx_Turn(LEDx LEDseq);
uint8_t LED_Detection(LEDx LEDseq);
void LED_SynchronizeW25Q64(LEDx LEDseq);
void LED_Config(void);
uint8_t LED_ReadWithW25Q(LEDx LEDseq);

#endif
