#include "exti.h"
#include "definepin.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_gpio.h"

/* EXTI Init----------------------------------------------*/
void EXTI_Init(void){
  /* ZB */
  GPIO_Init(GPIOD, GPIO_PIN_ZB_INT, GPIO_Mode_Out_OD_Low_Fast);
  GPIO_Init(GPIOB, GPIO_PIN_ZB_OUT, GPIO_Mode_In_FL_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Falling);
  
  /* Bell */
  GPIO_Init(GPIOB, GPIO_PIN_BELL_INT, GPIO_Mode_In_FL_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising);

  /* Switch */
  GPIO_Init(GPIOD, GPIO_PIN_SWITCH_INT, GPIO_Mode_In_PU_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);

  /* Reset */
  GPIO_Init(GPIOE, GPIO_PIN_RESET_INT, GPIO_Mode_In_PU_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);

  /* TEMPER */
  GPIO_Init(GPIOG, GPIO_PIN_TEMPER_INT, GPIO_Mode_In_FL_IT);
  EXTI_SelectPort(EXTI_Port_G);
  EXTI_SetHalfPortSelection(EXTI_HalfPort_G_MSB, ENABLE);
  EXTI_SetPortSensitivity(EXTI_Port_G, EXTI_Trigger_Falling);
}