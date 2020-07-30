/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_H
#define __AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

#define AUDIO_PIN_CLK GPIO_Pin_3
#define AUDIO_PIN_CS GPIO_Pin_4
#define AUDIO_PIN_DATA GPIO_Pin_5
#define AUDIO_PIN_COMMON GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5

void AUDIO_GPIO_Init(void);

void AUDIO_Send(uint8_t Address);

uint8_t AUDIO_Handle_Send(uint8_t Type_Command1, uint8_t Type_Command1_1, uint8_t Type_Command1_2, uint16_t Time_Sound);

#endif //__AUDIO_H