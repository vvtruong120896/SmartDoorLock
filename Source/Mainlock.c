#include "stm8l15x.h"
#include "stm8l15x_it.h"
#include "clock.h"
#include "delay.h"
#include "timer.h"
#include "Softwaretimer.h"
#include "eeprom.h"
#include "leddisplay.h"
#include "drivermotor.h"
#include "managerpower.h"
#include "defaultdevicecode.h"
#include "exti.h"
#include "sleep.h"
#include "tsm12.h"
#include "define.h"
#include "uart.h"
#include "ncp.h"
#include "zb_command.h"
#include "fingerprint_app.h"
#include "fingerprint.h" 
#include "mfcv520.h"
#include "audio.h"
#include "mainfunctionlist.h"

void Function_Init(void);

int main(void)
{
    Function_Init();
    while(1)
    {
        NCP_Proc();
        TIME_Proc();
        FingerPrint_Proc();

        Function_Check_All_Flag();
        Function_Check_Sleep();


        111
        111
        222
        44
        55566
        
    };
}

void Function_Init(void)
{
    CLK_Init(); SOFTWARETIMER_Init(); TIME_Init(); I2C_Config();

    TSM12_init();
    it_handle_irq_ext4_callback_init(TSM12_handle_digital_output_falling);
    TSM12_CallbackInit(app_main_handle_touch_btn_press);

    LED_Init(); Drivermotor_Init(); AUDIO_GPIO_Init(); EXTI_Init();

    NCP_Init(); FingerPrint_Init(FingerPrint_AppHandle);

    PCD_Init(); SLEEP_Mode_Init();

    REGISTER_DEVICE(DEFAULT_DEVICE_CODE);

    LED_Notifi_Init_Success(5);

    AUDIO_Send(BIG_SOUND);

    enableInterrupts();
}



