#include "managerpower.h"
#include "definepin.h"
#include "mfcv520.h"
#include "delay.h"

void LED_Control_Power_On(void)
{
    GPIO_SetBits(GPIOA, GPIO_PIN_LED_CTR_POWER); // Cap nguon cho led
    GPIO_SetBits(GPIOG, GPIO_PIN_CTR_LED); // Cap nguon cho led
}

void LED_Control_Power_Off(void)
{
    GPIO_ResetBits(GPIOA, GPIO_PIN_LED_CTR_POWER); // Cap nguon cho led
    GPIO_ResetBits(GPIOG, GPIO_PIN_CTR_LED); // Cap nguon cho led
}

void FINGER_Control_Power_On(void)
{
    GPIO_SetBits(GPIOC, GPIO_PIN_FINGER_CTR_POWER); // Cap nguon cho fingerprint
}

void FINGER_Control_Power_Off(void)
{
    GPIO_ResetBits(GPIOC, GPIO_PIN_FINGER_CTR_POWER); // Cap nguon cho fingerprint
}

void CARD_Control_Power_On(void)
{
    PCD_SoftPowerUp();
}

void CARD_Control_Power_Off(void)
{
   PCD_SoftPowerDown();
}

void KEYPAD_Control_Power_On(void)
{
}


