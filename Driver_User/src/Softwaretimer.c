#include "Softwaretimer.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_clk.h"

void SOFTWARETIMER_Init(void)
{
    /* Enable TIM1 CLK */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);

    /* Time base configuration */
    TIM1_TimeBaseInit(16, TIM1_CounterMode_Up, 999, 0);
    /* Clear TIM1 update flag */
    TIM1_ClearFlag(TIM1_FLAG_Update);
    /* Enable update interrupt */
    TIM1_ITConfig(TIM1_IT_Update, ENABLE);
    /* Enable TIM1 */
    TIM1_Cmd(ENABLE);
}
