/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEFINEPIN_H
#define __DEFINEPIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* Private define LED---------------------------------------------*/
#define GPIO_PIN_SH GPIO_Pin_0
#define GPIO_PIN_ST GPIO_Pin_1
#define GPIO_PIN_DS GPIO_Pin_2
#define GPIO_PIN_CTR_LED GPIO_Pin_3
#define LED_GPIO_PINS (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3)
#define GPIO_PIN_LED_CTR_POWER GPIO_Pin_7

/* Private define Motor---------------------------------------------*/
#define GPIO_PIN_MT1 GPIO_Pin_0
#define GPIO_PIN_MT2 GPIO_Pin_1
#define GPIO_PIN_MT GPIO_Pin_0|GPIO_Pin_1

/* Private define TSM ---------------------------------------------*/
#define GPIO_PIN_TSM_EN GPIO_Pin_4
#define GPIO_PIN_TSM_INT GPIO_Pin_4

/* Private define ---------------------------------------------*/
#define GPIO_PIN_BELL_INT GPIO_Pin_3

/* Private define ---------------------------------------------*/
#define GPIO_PIN_SWITCH_INT GPIO_Pin_2

/* Private define ---------------------------------------------*/
#define GPIO_PIN_RESET_INT GPIO_Pin_2

/* Private define ---------------------------------------------*/
#define GPIO_PIN_TEMPER_INT GPIO_Pin_6

/* Private define FINGER---------------------------------------------*/
#define GPIO_PIN_FINGER_CTR_POWER GPIO_Pin_4

/* Private define ZB */
#define GPIO_PIN_ZB_INT GPIO_Pin_1
#define GPIO_PIN_ZB_OUT GPIO_Pin_0

#endif //__DEFINEPIN_H