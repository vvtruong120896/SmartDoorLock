#include "sleep.h"
#include "stm8l15x_pwr.h"
#include "leddisplay.h"
#include "stm8l15x_gpio.h"
#include "definepin.h"
#include "mfcv520.h"
#include "managerpower.h"

extern volatile uint8_t Button_Card_Flag, Finger_Flag, Bell_Flag, Switch_Flag;
extern volatile uint8_t Timeout_Flag, Sleep_Flag;
extern volatile uint8_t DistanceTwiceReadCard;
extern volatile uint8_t zb_flagtogglenetwork, zb_flagchangelockstate, zb_flagjoinnetwork;

/* Sleep mode */
void SLEEP_Mode_Init(void){
  PWR_UltraLowPowerCmd(ENABLE);
  PWR_FastWakeUpCmd(ENABLE);
}

void SLEEP_Mode(void)
{
	Button_Card_Flag = 0; Finger_Flag = 0; zb_flagjoinnetwork = 0;
	Bell_Flag = 0; Timeout_Flag = 0; Sleep_Flag = 0; Switch_Flag = 0; 
	DistanceTwiceReadCard = 0;
	LED_Dispaly(0x00, 0x00);
	LED_Control_Power_Off();
	CARD_Control_Power_Off();
	halt();
}