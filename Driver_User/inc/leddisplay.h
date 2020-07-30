/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDDISPLAY_H
#define __LEDDISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"


/* LEDKEYPAD display */
void LED_Dispaly(uint8_t Number_1, uint8_t Number_2);

void LED_Notifi_Init_Success(uint8_t numberloop);

/* LED init */
void LED_Init(void);

#endif //__LEDDISPLAY_H