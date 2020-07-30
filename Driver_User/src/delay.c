/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "delay.h"

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/**
 * @func   delay_us
 * @brief  None
 * @param  
 * @retval None
 */
void delay_us(unsigned int  value)
{
    register unsigned int loops =  (unsigned int)(dly_const * value) ;
    
    while(loops)
    {
        asm ("nop");
        loops--;
    };
}

/**
 * @func   delay_ms
 * @brief  None
 * @param  
 * @retval None
 */
void delay_ms(unsigned int  value)
{
    while(value)
    {
        delay_us(1000);
        value--;
    };
}

// End File