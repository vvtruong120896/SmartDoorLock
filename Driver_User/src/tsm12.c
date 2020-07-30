 
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "stm8l15x.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_gpio.h"
#include "debug.h"
#include "stdbool.h"
#include "leddisplay.h"
#include "sw_i2c.h"
#include "tsm12.h"
#include "delay.h"
#include "audio.h"
#include <stddef.h>

typeTSM12_HandlelStateCallbackFunc  tsm12_handle_btn_press = NULL;

/******************************************************************************/
/*                     PRIVATE VARIABLE AND DEFINITION                        */
/******************************************************************************/

bool tsm12_scan_en_flag = false;

const uint8_t touch_key_valid_arr[TOUCH_PAD_NUMBER] =              \
    {                                                              \
       '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'  \
    };

#define PAD_ACTIVE_UNKNOWN     0

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

static uint8_t TSM12_read_byte(uint8_t address);
static void TSM12_write_byte(uint8_t reg_addr, uint8_t value);

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

 /**
 * @func   TSM12_read_byte
 * @brief  None
 * @param  
 * @retval  
 */
static uint8_t TSM12_read_byte(uint8_t address)
{
    uint8_t val = 0x00;
	
    SW_I2C_start();
    SW_I2C_write(TSM12_WR);
    SW_I2C_write(address);
	
    SW_I2C_start();
    SW_I2C_write(TSM12_RD);
    val = SW_I2C_read(I2C_NACK);
    SW_I2C_stop();
    return val;
}
  
/**
 * @func   TSM12_write_byte
 * @brief  None
 * @param  
 * @retval  
 */
static void TSM12_write_byte(uint8_t reg_addr, uint8_t value)
{
    SW_I2C_start();
    SW_I2C_write(TSM12_WR);
    SW_I2C_write(reg_addr);
    SW_I2C_write(value);
    SW_I2C_stop(); 
}

/**
 * @func   TSM12_init_pin_control
 * @brief  None
 * @param  
 * @retval Init Tsm12  
 */
static void TSM12_init_pin_control(void)
{
    SW_I2C_init();
    // Init Input Interrupt
    GPIO_Init(
        TSM12_interrupt_port,(GPIO_Pin_TypeDef)TSM12_interrupt_pin, GPIO_Mode_In_PU_IT
        );
    
    EXTI_SetPinSensitivity(TSM12_interrupt_pin_ext, EXTI_Trigger_Falling);
    
    // TSM12 Reset Pin
    GPIO_Init(
        TSM12_I2C_rst_port,(GPIO_Pin_TypeDef)TSM12_I2C_rst_pin, GPIO_Mode_Out_PP_High_Fast
        );
    GPIO_WriteBit(TSM12_I2C_rst_port, (GPIO_Pin_TypeDef)TSM12_I2C_rst_pin, SET);
    
    // Init Output Enable (Enable I2C)
    GPIO_Init(
        TSM12_I2C_enable_port, (GPIO_Pin_TypeDef)TSM12_I2C_enable_pin, GPIO_Mode_Out_PP_High_Fast
        );
    GPIO_WriteBit(TSM12_I2C_enable_port, (GPIO_Pin_TypeDef)TSM12_I2C_enable_pin, RESET);
}

/**
 * @func   TSM12_init
 * @brief  None
 * @param  
 * @retval Init Tsm12  
 */
void TSM12_init(void)
{
    TSM12_init_pin_control();
    
    delay_ms(100);
    
    // Init Register
    TSM12_write_byte(
              TSM12_CTRL2, 0x0F
           );

    TSM12_write_byte(
              TSM12_CTRL2, 0x07
           );
    
    // Sensitivity Control Register Config 
    TSM12_write_byte(
              TSM12_SENSITIVITY_1, TSM12_SENSITIVITY_DEFAULT
           );
    TSM12_write_byte(
              TSM12_SENSITIVITY_2, TSM12_SENSITIVITY_DEFAULT
           );
    TSM12_write_byte(
              TSM12_SENSITIVITY_3, TSM12_SENSITIVITY_DEFAULT
           );
    TSM12_write_byte(
              TSM12_SENSITIVITY_4, TSM12_SENSITIVITY_DEFAULT
           );
    TSM12_write_byte(
              TSM12_SENSITIVITY_5, TSM12_SENSITIVITY_DEFAULT
           );
    TSM12_write_byte(
              TSM12_SENSITIVITY_6, TSM12_SENSITIVITY_DEFAULT
           );
    
    // CTRL 1: Interrupt is on middle or high output, 
    // fast mode selection, First Touch Control - 5 seconds
    TSM12_write_byte(
             TSM12_CTRL1, 0x88
           );
    
    // Channel Reference Reset Control Register Config 
    TSM12_write_byte(
              TSM12_REF_RST_1, 0xFF
           );
    TSM12_write_byte(
              TSM12_REF_RST_2, 0xFF
           );
    
    delay_ms(100);

    TSM12_write_byte(
              TSM12_REF_RST_1, 0x00
           );
    TSM12_write_byte(
              TSM12_REF_RST_2, 0x00
           );
    
    // Channel Sensing Control Register Config
    TSM12_write_byte(
              TSM12_CH_HOLD_1, 0x00
           );
    TSM12_write_byte(
              TSM12_CH_HOLD_2, 0x00
           );
    
}
 
/**
 * @func   TSM12_key_read
 * @brief  None
 * @param  
 * @retval None
 */
static uint8_t TSM12_key_read(void)
{
    volatile uint8_t temp[3];
    volatile uint8_t *p;
    volatile uint8_t key_value = NULL;
    
    p = temp;
    
    *p = TSM12_read_byte(TSM12_OUTPUT_1);         // 1 - 4
    *(p + 1) = TSM12_read_byte(TSM12_OUTPUT_2);   // 5 - 8
    *(p + 2) = TSM12_read_byte(TSM12_OUTPUT_3);   // 9 - 12
    
    if(*p > 0)
    {
        switch(*p)
        {
            case 0x03:  
            case 0x02:
            case 0x01:
              {
                key_value = '9'; 
                break;
              }
            case 0x0C:  
            case 0x08:
            case 0x04:
              {
                key_value = '6'; 
                break;
              }
            case 0x30:  
            case 0x20:
            case 0x10:
              {
                key_value = '3'; 
                break;
              }
            case 0xC0: 
            case 0x80:
            case 0x40:
              {
                key_value = '1'; 
                break;
              }
            default: 
              {
                key_value = PAD_ACTIVE_UNKNOWN;
                break; 
              }
        }  
    }
    else if(*(p+1) > 0)
    {
        switch(*(p + 1))
        {
            case 0x03: 
            case 0x02:
            case 0x01:
              {
                key_value = '4'; 
                break;
              }
            case 0x0C:  
            case 0x08:
            case 0x04:
              {
                key_value = '7';
                break;
              }
            case 0x30: 
            case 0x20:
            case 0x10:
              {
                key_value = '*'; 
                break;
              }
            case 0xC0: 
            case 0x80:
            case 0x40:
              {
                key_value = '2'; 
                break;
              }
            default: 
              {
                key_value = PAD_ACTIVE_UNKNOWN; 
                break;
              }
        }
    }
    else{
        switch(*(p + 2))
        {
            case 0x03:  
            case 0x02:
            case 0x01:
              {
                key_value = '5'; 
                break;
              }
            case 0x0C:  
            case 0x08:
            case 0x04:
              {
                key_value = '8'; 
                break;
              }
            case 0x30:  
            case 0x20:
            case 0x10:
              {
                key_value = '0'; 
                break;
              }
            case 0xC0:  
            case 0x80:
            case 0x40:
              {
                key_value = '#'; 
                break;
              }
            default: 
              {
                key_value = PAD_ACTIVE_UNKNOWN; 
                break;
              }
        }
    }
    return key_value;
}

/**
 * @func   TSM12_key_pad_scan
 * @brief  None
 * @param  
 * @retval None
 */
uint8_t TSM12_key_pad_scan(void)
{
    uint8_t pad_active = PAD_ACTIVE_UNKNOWN;
    if(tsm12_scan_en_flag == true)
    {
        pad_active = TSM12_key_read();
        tsm12_scan_en_flag = false;
    }
    return pad_active;
}

/**
 * @func   TSM12_HandleDigitalOutputRising
 * @brief  None
 * @param  
 * @retval None
 */
void TSM12_handle_digital_output_falling(void)
{
    tsm12_scan_en_flag = true;
} 
       
/**
 * @func   TSM12_task_manager
 * @brief  None
 * @param  
 * @retval None
 */
uint8_t TSM12_task_manager(void)
{
    uint8_t pad_active = PAD_ACTIVE_UNKNOWN;
    uint8_t i = 0;
    
    if(tsm12_scan_en_flag == true) 
    {
        pad_active = TSM12_key_pad_scan();
        for(i = 0; i < TOUCH_PAD_NUMBER; i++)
        {
            if(pad_active == touch_key_valid_arr[i])
            {
               if(tsm12_handle_btn_press != NULL)
               {
                   tsm12_handle_btn_press(pad_active);
               }
               break;
            }
        }   
        if(i == TOUCH_PAD_NUMBER)
        {
            if(tsm12_handle_btn_press != NULL)
            {
                tsm12_handle_btn_press(PAD_ACTIVE_UNKNOWN);
            }
        }
    }
    return pad_active;
} 
/**
 * @func   TSM12_task_manager
 * @brief  None
 * @param  
 * @retval None
 */
void TSM12_CallbackInit(typeTSM12_HandlelStateCallbackFunc 
          typeTSM12_HandlelStateCallbackInit)
{
    if (typeTSM12_HandlelStateCallbackInit != NULL)
    {
        tsm12_handle_btn_press = typeTSM12_HandlelStateCallbackInit;
    }
}

/**
 * @func   TSM12_key_pad_polling
 * @brief  None
 * @param  
 * @retval None
 */
uint8_t TSM12_key_pad_polling(void)
{
    return TSM12_key_read();;
}

/**
 * @func    app_main_handle_touch_btn_press
 * @brief  None
 * @param  None
 * @retval None
 */
void app_main_handle_touch_btn_press(char btn)
{
    
    switch(btn)
    {
        case '0':
            AUDIO_Send(201);
            LED_Dispaly(0x0B, 0xFF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0B, 0xFF);
            break;
        case '1':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xFE);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xFE);
            break;
        case '2':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xFD);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xFD);
            break;
        case '3':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xFB);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xFB);
            break;
        case '4':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xF7);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xF7);
            break;
        case '5':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xEF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xEF);
            break;
        case '6':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xDF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xDF);
            break;
        case '7':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0xBF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0xBF);
            break;
        case '8':
            AUDIO_Send(201);
            LED_Dispaly(0x0F, 0x7F);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0F, 0x7F);
            break;
        case '9':
            AUDIO_Send(201);
            LED_Dispaly(0x0E, 0xFF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0E, 0xFF);
            break;
        case '*':
            AUDIO_Send(201);
            LED_Dispaly(0x0D, 0xFF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x0D, 0xFF);
            break;
        case '#':
            AUDIO_Send(201);
            LED_Dispaly(0x07, 0xFF);
            delay_ms(TSM12_Timeout);
            LED_Dispaly(0x07, 0xFF);
            break;
            
        default: 
            LED_Dispaly(0x0F, 0xFF);
            break;
    } 
}
// End File