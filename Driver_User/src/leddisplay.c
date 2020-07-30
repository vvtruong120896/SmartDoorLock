#include "leddisplay.h"
#include "definepin.h"
#include "delay.h"
#include "managerpower.h"
#include "stm8l15x_gpio.h"

/* LEDKEYPAD display */
void LED_Dispaly(uint8_t Number_1, uint8_t Number_2)
{
  uint8_t Temp = 0;
  for(uint8_t i = 0; i < 8; i++)
  {
    Temp = (Number_1 << i);
    if(Temp & 0x80)
      GPIO_SetBits(GPIOG, GPIO_PIN_DS);
    else
      GPIO_ResetBits(GPIOG, GPIO_PIN_DS);
    
    GPIO_ResetBits(GPIOG, GPIO_PIN_SH);
    GPIO_SetBits(GPIOG, GPIO_PIN_SH);
  }
  for(uint8_t i = 0; i < 8; i++)
  {
    Temp = (Number_2 << i);
    if(Temp & 0x80)
      GPIO_SetBits(GPIOG, GPIO_PIN_DS);
    else
      GPIO_ResetBits(GPIOG, GPIO_PIN_DS);
    
    GPIO_ResetBits(GPIOG, GPIO_PIN_SH);
    GPIO_SetBits(GPIOG, GPIO_PIN_SH);
  }
  
  GPIO_ResetBits(GPIOG, GPIO_PIN_ST);
  GPIO_SetBits(GPIOG, GPIO_PIN_ST);
}

void LED_Notifi_Init_Success(uint8_t numberloop)
{
  LED_Control_Power_On();
  for(uint8_t i = 0; i < numberloop; i++)
  {
    LED_Dispaly(0x00, 0x01);
    delay_ms(100);
    LED_Dispaly(0x00, 0x02);
    delay_ms(100);
    LED_Dispaly(0x00, 0x04);
    delay_ms(100);
    LED_Dispaly(0x00, 0x20);
    delay_ms(100);
    LED_Dispaly(0x01, 0x00);
    delay_ms(100);
    LED_Dispaly(0x08, 0x00);
    delay_ms(100);
    LED_Dispaly(0x04, 0x00);
    delay_ms(100);
    LED_Dispaly(0x02, 0x00);
    delay_ms(100);
    LED_Dispaly(0x00, 0x40);
    delay_ms(100);
    LED_Dispaly(0x00, 0x08);
    delay_ms(100);
  }
}

/* LED init */
void LED_Init(void)
{
  GPIO_Init(GPIOG, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(GPIOA, GPIO_PIN_LED_CTR_POWER, GPIO_Mode_Out_PP_Low_Fast);
}
