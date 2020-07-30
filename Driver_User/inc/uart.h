#ifndef _UART_H_
#define _UART_H_

#include "stddef.h"

#include "stm8l15x.h"

#define UART_COM_MAX    3 
#define COM1  USART1
#define COM2  USART2
#define COM3  USART3

#define UART_RSP_TIMEOUT                        100

#define UART_INITIALIZE                                                 \
{                                                                       \
    {COM1,NULL                      ,TRUE,CLK_Peripheral_USART1 },      \
    {COM2,NULL                      ,TRUE,CLK_Peripheral_USART2},      \
    {COM3,NULL                      ,FALSE,CLK_Peripheral_USART3}       \
}

enum
{
    UART_TX_ENABLE,
    UART_TX_DISABLE
};

enum
{
    UART_RX_ENABLE,
    UART_RX_DISABLE,
};

typedef enum UART_Event_t
{
    UART_EV_RX_INT,
    UART_EV_TX_INT,
    UART_EV_MAX,
} UART_Event_t;

typedef enum UARTx_t
{
    UART1,
    UART2,
    UART3,
    UART_MAX
} UARTx_t;

typedef void (*UART_EventHandle_t)(void);

typedef struct UARTx_Control_t
{
    USART_TypeDef*          com;
    REMAP_Pin_TypeDef       pinRemap;     
    u8                      isRemap;
    CLK_Peripheral_TypeDef  clkPeriph;
    u32                     baudrate;
    FunctionalState         en;
    UART_EventHandle_t      pEvent[UART_EV_MAX];
} UARTx_Control_t;



void UART_Init(UARTx_t index,u32 baudrate,u8 txEnable,u8 rxEnable);
void UART_Deinit(UARTx_t uartx);


void UART_RegisterEvent(UARTx_t uartx,UART_Event_t ev,UART_EventHandle_t pHandle);
void UART_RegisterEvent(UARTx_t uartx,UART_Event_t ev,UART_EventHandle_t pHandle);

void UART_Enable(UARTx_t uartx,FunctionalState state);

u8   UART_GetData(UARTx_t index);
void UART_SendData(UARTx_t,u8* buff,u16 len);

void UART_CommonInteruptHandle(void);

#endif