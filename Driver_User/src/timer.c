#include "timer.h"
#include "stddef.h"

#include "stm8l15x_tim4.h"

static TIMx_t timeEvent[TIME_EVENT_MAX];

static u32 system_time_ms=0;

void TIME_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
    
    TIM4_TimeBaseInit(TIM4_Prescaler_128,(u8)(CLK_GetClockFreq()/128000));
    TIM4_ITConfig(TIM4_IT_Update,ENABLE);

    for (u8 i = 0; i < TIME_EVENT_MAX; i++)
    {
        timeEvent[i].userHandle=NULL;
    }

    TIM4_Cmd(ENABLE);
}

void TIME_DeInit(void)
{
    TIM4_DeInit();
}


void TIME_Proc(void)
{
    for(u8 i=0;i<TIME_EVENT_MAX;i++)
    {
        if(TIME_ExceedMs(timeEvent[i].startTime,timeEvent[i].timeLenght))
        {
            timeEvent[i].startTime=TIME_GetCurrentTime();
            if(timeEvent[i].userHandle!=NULL)
            {               
                timeEvent[i].userHandle();
            }
        }
    }
}

void TIME_Active(timerExpriseHandle_t userHandle,u32 ms)
{
    if((ms==0)&&(userHandle!=NULL))
    {
        userHandle();
    }else
    {
        for(u8 i=0;i<TIME_EVENT_MAX;i++)
        {
            if(timeEvent[i].userHandle==NULL)
            {
                timeEvent[i].startTime=TIME_GetCurrentTime();
                timeEvent[i].timeLenght=ms;
                timeEvent[i].userHandle=userHandle;
                break;
            }
        }
    } 
}

void TIME_InActive(timerExpriseHandle_t userHandle)
{
    for(u8 i=0;i<TIME_EVENT_MAX;i++)
    {
        if(timeEvent[i].userHandle==userHandle)
        {           
            timeEvent[i].userHandle=NULL;
            break;
        }
    }
}

u32  TIME_GetCurrentTime(void)
{
  return system_time_ms;
}

u32 TIME_GetTimeLenght(u32 startTime)
{
    u32 currentTime;

    currentTime=TIME_GetCurrentTime();
    if(currentTime >=startTime)
    {
        return  (currentTime-startTime);
    }

    return  (currentTime+(0xffffffff-startTime));
}

u8   TIME_ExceedMs(u32 startTime,u32 ms)
{
    if(TIME_GetTimeLenght(startTime)>=ms)
    {
        return 1;
    }

    return 0;
}

void TIM_InterruptHandle(void)
{
    system_time_ms++;
}