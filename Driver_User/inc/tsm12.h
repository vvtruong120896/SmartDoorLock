
/* Define to prevent recursive inclusion ---------------------*/

#ifndef __TSM12_I2C_H
#define __TSM12_I2C_H

/* Includes --------------------------------------------------*/

#include "stm8l15x.h"

/* Callback --------------------------------------------------*/

typedef void (*typeTSM12_HandlelStateCallbackFunc)(char);

/* Private typedef -------------------------------------------*/
/* Private define  -------------------------------------------*/

#define I2C_SUCCESS   0x01
#define I2C_FAIL      0x00

#define TSM12_ID_SEL_GND_ADDR     0xD0
#define TSM12_ID_SEL_VCC_ADDR     0xF0
 
#define I2C_SLAVE_ADDRESS7        TSM12_ID_SEL_GND_ADDR

// TSM12 Register Definition

#define TSM12_SENSITIVITY_1       0x02
#define TSM12_SENSITIVITY_2       0x03
#define TSM12_SENSITIVITY_3       0x04
#define TSM12_SENSITIVITY_4       0x05
#define TSM12_SENSITIVITY_5       0x06
#define TSM12_SENSITIVITY_6       0x07

#define TSM12_CTRL1               0x08
#define TSM12_CTRL2               0x09

#define TSM12_REF_RST_1           0x0A
#define TSM12_REF_RST_2           0x0B

#define TSM12_CH_HOLD_1           0x0C
#define TSM12_CH_HOLD_2           0x0D

#define TSM12_CAL_HOLD_1          0x0E
#define TSM12_CAL_HOLD_2          0x0F

#define TSM12_OUTPUT_1            0x10
#define TSM12_OUTPUT_2            0x11
#define TSM12_OUTPUT_3            0x12

// Defaul Value 
#define TSM12_SENSITIVITY_DEFAULT 0x44

// I2C Enable/Disble IO Pin Control 
#define TSM12_I2C_enable_port  GPIOG
#define TSM12_I2C_enable_pin   GPIO_Pin_4

// TSM12 Reset IO Pin Control 
#define TSM12_I2C_rst_port     GPIOD
#define TSM12_I2C_rst_pin      GPIO_Pin_5

// TSM12 Interrupt Input IO Pin 
#define TSM12_interrupt_port         GPIOD
#define TSM12_interrupt_pin          GPIO_Pin_4

#define TSM12_interrupt_pin_ext      EXTI_Pin_4


#define TSM12_7BIT_GND_ADDR    0xD0
#define TSM12_7BIT_VDD_ADDR    0xF0

#define TSM12_WR   0xD0
#define TSM12_RD   0xD1

#define TOUCH_PAD_NUMBER    12

#define TSM12_Timeout 150
/* Export variable ------------------------------------------*/



/* Export functions ------------------------------------------*/

void TSM12_init(void);
void TSM12_handle_digital_output_falling(void);
uint8_t TSM12_task_manager(void);
void TSM12_CallbackInit(typeTSM12_HandlelStateCallbackFunc typeTSM12_HandlelStateCallbackInit);
uint8_t TSM12_key_pad_polling(void);
void app_main_handle_touch_btn_press(char btn);


#endif  /* __TSM12_I2C_H */
