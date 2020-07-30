
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "sw_i2c.h"
#include "delay.h"
#include "tsm12.h"

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                          PRIVATE FUNCTIONS DECLERATION                     */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
/**
 * @func   SW_I2C_OUT
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_OUT(void)    
{  
   GPIOD->CR2 &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin)); 
   GPIOD->DDR &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin));  
   GPIOD->CR1 &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin));  
   
   GPIO_Init(SW_I2C_port, SDA_pin, GPIO_Mode_Out_PP_Low_Fast);  
   GPIO_Init(SW_I2C_port, SCL_pin, GPIO_Mode_Out_PP_Low_Fast);
}

/**
 * @func   SW_I2C_IN
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_IN(void)
{      
   GPIOD->CR2 &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin)); 
   GPIOD->DDR &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin));  
   GPIOD->CR1 &= (uint8_t)(~(SDA_pin | SCL_pin | TSM12_I2C_rst_pin));  
   
   GPIO_Init(SW_I2C_port, SDA_pin, GPIO_Mode_In_FL_No_IT);    
   GPIO_Init(SW_I2C_port, SCL_pin, GPIO_Mode_Out_PP_Low_Fast);
}

/**
 * @func   SW_I2C_init
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_init(void)
{
    SW_I2C_OUT();
    delay_ms(10);
    SDA_HIGH();
    SCL_HIGH();
}

/**
 * @func   SW_I2C_start
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_start(void)
{
    SW_I2C_OUT();
    SDA_HIGH();
    SCL_HIGH();
    delay_us(40);
    SDA_LOW();
    delay_us(40);
    SCL_LOW();
}

/**
 * @func   SW_I2C_stop
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_stop(void)
{
    SW_I2C_OUT();
    SDA_LOW();
    SCL_LOW();
    delay_us(40);
    SDA_HIGH();
    SCL_HIGH();
    delay_us(40);
}

/**
 * @func   SW_I2C_read
 * @brief  None
 * @param  
 * @retval None
 */
unsigned char SW_I2C_read(unsigned char ack)
{
    unsigned char i = 0x08;
    unsigned char j = 0x00;

    SW_I2C_IN();

    while(i > 0x00)
    {
        SCL_LOW();
        delay_us(20);
        SCL_HIGH();
        delay_us(20);
        j <<= 1;

        if(SDA_IN() != 0x00)
        {
            j++;
        }

        delay_us(10);
        i--;
    };

    switch(ack)
    {
        case I2C_ACK:
        {
            SW_I2C_ACK_NACK(I2C_ACK);;
            break;
        }
        default:
        {
            SW_I2C_ACK_NACK(I2C_NACK);;
            break;
        }
    }

    return j;
}

/**
 * @func   SW_I2C_write
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_write(unsigned char value)
{
    unsigned char i = 0x08;

    SW_I2C_OUT();
    SCL_LOW();

    while(i > 0x00)
    {
        if(((value & 0x80) >> 0x07) != 0x00)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }
        
        value <<= 1;
        delay_us(20);
        SCL_HIGH();
        delay_us(20);
        SCL_LOW();
        delay_us(20);
        i--;
    };
}

/**
 * @func   SW_I2C_ACK_NACK
 * @brief  None
 * @param  
 * @retval None
 */
void SW_I2C_ACK_NACK(unsigned char mode)
{
    SCL_LOW();
    SW_I2C_OUT();

    switch(mode)
    {
        case I2C_ACK:
        {
            SDA_LOW();
            break;
        }
        default:
        {
            SDA_HIGH();
            break;
        }
    }

    delay_us(20);
    SCL_HIGH();
    delay_us(20);
    SCL_LOW();
}

/**
 * @func   SW_I2C_wait_ACK
 * @brief  None
 * @param  
 * @retval None
 */
uint8_t SW_I2C_wait_ACK(void)
{
    uint16_t timeout = 0;

    SW_I2C_IN();

    SDA_HIGH();
    delay_us(10);
    SCL_HIGH();
    delay_us(10);

    while(SDA_IN() != 0x00)
    {
        timeout++;

        if(timeout > I2C_TIMEOUT_DEFAULT)
        {
            SW_I2C_stop();
            return 1;
        }
    };
    
    SCL_LOW();

    return 0x00;
}

// End File