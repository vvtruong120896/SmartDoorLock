#include "audio.h"
#include "delay.h"
#include "define.h"
#include "tsm12.h"

extern volatile uint16_t Timeout_Sound_Count, Start_Count;
extern uint8_t Check_Language;


void AUDIO_GPIO_Init(void)
{
    GPIO_Init(GPIOA, AUDIO_PIN_COMMON, GPIO_Mode_Out_PP_High_Fast);
}

void AUDIO_Send(uint8_t Address)
{
    delay_ms(20);
    GPIO_ResetBits(GPIOA, AUDIO_PIN_CS);
    delay_ms(5);
    for(uint8_t i = 0; i < 8; i++)
    {
        GPIO_ResetBits(GPIOA, AUDIO_PIN_CLK);
        if(Address & 0x01)
        {
            GPIO_SetBits(GPIOA, AUDIO_PIN_DATA);
        }
        else
        {
            GPIO_ResetBits(GPIOA, AUDIO_PIN_DATA);
        }
        Address >>= 1;
        delay_us(100);
        GPIO_SetBits(GPIOA, AUDIO_PIN_CLK);
        delay_us(100);
        
    }
    GPIO_SetBits(GPIOA, AUDIO_PIN_CS);
}

uint8_t AUDIO_Handle_Send(uint8_t Type_Command1, uint8_t Type_Command1_1, uint8_t Type_Command1_2, uint16_t Time_Sound)
{
    uint8_t Key = 0;
    if(Check_Language == 0)
    {
        AUDIO_Send(Type_Command1);
        Start_Count = 1; Timeout_Sound_Count = 0;
        do
        {
            Key = TSM12_key_pad_polling();
        } while ((Key == 0) && (Timeout_Sound_Count < Time_Sound));
        if(Key != 0) 
        {
            Start_Count = 0;
            return Key;
        }
        else
        {
            if((Type_Command1_1 == 0) || (Type_Command1_2 == 0)) 
            {
                return 0;
            }
            else
            {
                AUDIO_Send(Type_Command1_1);
                Start_Count = 1; Timeout_Sound_Count = 0;
                do
                {
                    Key = TSM12_key_pad_polling();
                } while ((Key == 0) && (Timeout_Sound_Count < MIN_TIME_SOUND));
                if(Key != 0) 
                {
                    Start_Count = 0;
                    return Key;
                }
                else
                {
                    AUDIO_Send(Type_Command1_2);
                    Start_Count = 1; Timeout_Sound_Count = 0;
                    do
                    {
                        Key = TSM12_key_pad_polling();
                    } while ((Key == 0) && (Timeout_Sound_Count < MIN_TIME_SOUND));
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        return Key;
                    }
                    else
                    {
                        return 0;
                    }
                    
                }
            }
            
        }
    }
    else if(Check_Language == 1)
    {
        AUDIO_Send(Type_Command1+1);
        Start_Count = 1; Timeout_Sound_Count = 0;
        do
        {
            Key = TSM12_key_pad_polling();
        } while ((Key == 0) && (Timeout_Sound_Count < Time_Sound));
        if(Key != 0) 
        {
            Start_Count = 0;
            return Key;
        }
        else
        {
            if((Type_Command1_1 == 0) || (Type_Command1_2 == 0)) 
            {
                return 0;
            }
            else
            {
                AUDIO_Send(Type_Command1_1+1);
                Start_Count = 1; Timeout_Sound_Count = 0;
                do
                {
                    Key = TSM12_key_pad_polling();
                } while ((Key == 0) && (Timeout_Sound_Count < MIN_TIME_SOUND));
                if(Key != 0) 
                {
                    Start_Count = 0;
                    return Key;
                }
                else
                {
                    AUDIO_Send(Type_Command1_2+1);
                    Start_Count = 1; Timeout_Sound_Count = 0;
                    do
                    {
                        Key = TSM12_key_pad_polling();
                    } while ((Key == 0) && (Timeout_Sound_Count < MIN_TIME_SOUND));
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        return Key;
                    }
                    else
                    {
                        return 0;
                    }
                    
                }
            }
            
        }
    }
    return 0;
    
}
