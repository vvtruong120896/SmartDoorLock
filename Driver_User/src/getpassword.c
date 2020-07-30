#include "getpassword.h"
#include "define.h"
#include "tsm12.h"
#include "leddisplay.h"
#include "delay.h"
#include "mfcv520.h"
#include "stm8l15x_usart.h"
#include <stddef.h>
#include <string.h>

extern volatile uint8_t Sleep_Flag;

/* GET Password */
uint8_t GET_Password(uint8_t* Password_Array)
{
    memset(Password_Array, 0, 16);
    uint8_t Key = 0, Count_Key = 0;
    do
    {
        while(Key == 0)
        {
            Key = TSM12_task_manager();
            delay_ms(100);
            if(Sleep_Flag == 1)
            {
                return BACK;
            }
        }
        if(Key == '*')
        {
            return BACK;
        }
        else
        {
            if(Key == '#')
            {
                return REPEAT;
            }
            Password_Array[Count_Key] = Key; Key = 0;
            Count_Key++;
        }
    }while(Key != '#');
    return 0;
}

/* GET Key */
uint8_t GET_Key(void)
{
    uint8_t Keyc = 0;
    while(Keyc == 0)
    {
        Keyc = TSM12_key_pad_polling();
        delay_ms(100);
        if(Sleep_Flag == 1)
            return BACK;
    }
    return Keyc;
}
/* GET Card */
uint8_t GET_Card(uint8_t* Card_Array)
{
    uint8_t Key = 0;
    memset(Card_Array, 0x00, SIZEOFFRAME);
    do
    {
        Key = TSM12_key_pad_polling();
        if(Key == '*') return Key;
        READ_Card(Card_Array);
        delay_ms(100);
    } while ((Card_Array[0] == 0x00) && (Sleep_Flag == 0));
    if(Card_Array[0] != 0x00)
    {
        return 1;
    }
    else if(Sleep_Flag == 1)
    {
        return 2;
    }
    return 0;

}