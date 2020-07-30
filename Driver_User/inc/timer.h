#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm8l15x.h"


#define TIME_EVENT_MAX      10

typedef void (*timerExpriseHandle_t)(void);

typedef struct TIMx_t
{
    u32 startTime;
    u32 timeLenght;
    timerExpriseHandle_t userHandle;
}TIMx_t;

void TIME_Init(void);
void TIME_DeInit(void);
void TIME_Proc(void);

u32  TIME_GetCurrentTime(void);

void TIME_Active(timerExpriseHandle_t userHandle,u32 ms);
void TIME_InActive(timerExpriseHandle_t userHandle);

u32  TIME_GetTimeLenght(u32 startTime);
u8   TIME_ExceedMs(u32 startTime,u32 ms);

// Put in interrupt TIM4 file it.c
void TIM_InterruptHandle(void);

#endif