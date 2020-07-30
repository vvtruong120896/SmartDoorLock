/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MANAGERPOWER_H
#define __MANAGERPOWER_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

void LED_Control_Power_On(void);

void LED_Control_Power_Off(void);

void FINGER_Control_Power_On(void);

void FINGER_Control_Power_Off(void);

void CARD_Control_Power_On(void);

void CARD_Control_Power_Off(void);

void KEYPAD_Control_Power(void);


#endif //__MANAGERPOWER_H