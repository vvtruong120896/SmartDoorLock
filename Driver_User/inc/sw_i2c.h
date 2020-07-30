
#ifndef __SW_I2C_H_
#define __SW_I2C_H_

/******************************************************************************/
/*                            INCLUDE FILES                                   */
/******************************************************************************/

#include "stm8l15x.h"

/******************************************************************************/
/*                       EXPORT TYPE AND DEFINITION                           */
/******************************************************************************/

#define SW_I2C_port	GPIOD

#define SDA_pin         GPIO_Pin_7
#define SCL_pin         GPIO_Pin_6
      
/*
// SDA Output, Push - Pull
#define SW_I2C_OUT()    do{                                \
                            SW_I2C_port->DDR |= SDA_pin;   \
                            SW_I2C_port->CR1 |= SDA_pin;   \
                            SW_I2C_port->DDR |= SCL_pin;   \
                            SW_I2C_port->CR1 |= SCL_pin;   \
                        }while(0)
     
// SDA Input, Floating Input
#define SW_I2C_IN()     do{                                            \
                            SW_I2C_port->DDR &= (uint8_t)(~(SDA_pin)); \
                            SW_I2C_port->CR1 &= (uint8_t)(~(SDA_pin)); \
                            SW_I2C_port->DDR |= SCL_pin;               \
                            SW_I2C_port->CR1 |= SCL_pin;               \
                        }while(0)            
*/        
     
                          
                          

#define SDA_HIGH()      GPIO_WriteBit(SW_I2C_port, SDA_pin, SET);
#define SDA_LOW()       GPIO_WriteBit(SW_I2C_port, SDA_pin, RESET);

#define SCL_HIGH()      GPIO_WriteBit(SW_I2C_port, SCL_pin, SET);
#define SCL_LOW()      	GPIO_WriteBit(SW_I2C_port, SCL_pin, RESET);

#define SDA_IN()        GPIO_ReadInputDataBit(SW_I2C_port, SDA_pin)

#define I2C_ACK         0xFF
#define I2C_NACK        0x00

#define I2C_TIMEOUT_DEFAULT     1000

/******************************************************************************/
/*                              EXPORT FUNCTION                               */
/******************************************************************************/

void SW_I2C_init(void);
void SW_I2C_start(void);
void SW_I2C_stop(void);
unsigned char SW_I2C_read(unsigned char ack);
void SW_I2C_write(unsigned char value);
void SW_I2C_ACK_NACK(unsigned char mode);
unsigned char SW_I2C_wait_ACK(void);

void SW_I2C_OUT(void);
void SW_I2C_IN(void);

#endif /* __SW_I2C_H_ */