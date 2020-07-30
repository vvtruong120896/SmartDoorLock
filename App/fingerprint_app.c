#include "fingerprint_app.h"
#include "ncp.h"
#include "stdio.h"
#include "definepin.h"
#include "stm8l15x_gpio.h"
#include "delay.h"
#include "ncp.h"
#include "drivermotor.h"
#include "stdbool.h"
#include "../Mid/fingerprint/fingerprint.h"

//extern volatile uint8_t Check_Motor;
//extern uint8_t Buffer_vtupdateunlock[6];
extern volatile uint8_t Finger_Flag;
//extern volatile uint8_t Finger_FlagError;
extern bool Check_Status_Success;
extern volatile uint8_t Sleep_Flag;


void FingerPrint_AppHandle(u8 event, u8* par, u8 par_len)
{
    u8 errCode=ERROR_SUCCESS;

    errCode=par[0];

    switch (event)
    {
        case FINGERPRINT_HANDSHAKE_END:        
            if(errCode==ERROR_SUCCESS) 
            break;
        case FINGERPRINT_ENROLL_END:
            break;
        case FINGERPRINT_SEARCH_END:
            if(errCode==ERROR_SUCCESS)
            {   
                u16 userID=0;
                userID=(u16)(par[1]<<8)+ (u16)par[2];
                if(userID == 0)
                {
                    Finger_Flag = 1;
                    break;
                }               
            }
            break;
        default:
            break;
    }
}
