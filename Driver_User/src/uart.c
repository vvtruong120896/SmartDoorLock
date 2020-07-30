#include "uart.h"

#include "stm8l15x_usart.h"

UARTx_Control_t UART_Ctl[UART_MAX]= UART_INITIALIZE;

void UART_Init(UARTx_t index,u32 baudrate,u8 txEnable,u8 rxEnable)
{
    GPIO_TypeDef uartGPIO;
    u8 mode=0;

    UART_Ctl[index].baudrate=baudrate;
    UART_Ctl[index].en=DISABLE;

    if(txEnable==UART_TX_ENABLE)
    {
        mode |=USART_Mode_Tx;
    }

    if(rxEnable==UART_RX_ENABLE)
    {
        mode |=USART_Mode_Rx;
    }

    CLK_PeripheralClockConfig(UART_Ctl[index].clkPeriph,ENABLE);

    if(UART_Ctl[index].isRemap==TRUE)
    {
        SYSCFG_REMAPPinConfig(UART_Ctl[index].pinRemap,ENABLE);
    }
    
    USART_Init(UART_Ctl[index].com,baudrate,USART_WordLength_8b,USART_StopBits_1,
               USART_Parity_No,mode);

    UART_Enable(index,DISABLE);

}

void UART_Deinit(UARTx_t uartx)
{
    USART_DeInit(UART_Ctl[uartx].com);
}

void UART_Enable(UARTx_t uartx,FunctionalState state)
{
    UART_Ctl[uartx].en=state;
    USART_Cmd(UART_Ctl[uartx].com,state);
}

void UART_RegisterEvent(UARTx_t uartx,UART_Event_t ev,UART_EventHandle_t pHandle)
{
    if(ev>=UART_EV_MAX)
    {
        return;
    }

    UART_Ctl[uartx].pEvent[ev]=pHandle;

    switch (ev)
    {
    case UART_EV_TX_INT:
        USART_ITConfig(UART_Ctl[uartx].com,USART_IT_TC,ENABLE);
        break;
    case UART_EV_RX_INT:
        USART_ITConfig(UART_Ctl[uartx].com,USART_IT_RXNE,ENABLE);
        break;
    default:
        break;
    }
}
void UART_UnRegisterEvent(UARTx_t uartx,UART_Event_t ev)
{
    if(ev>=UART_EV_MAX)
    {
        return;
    }

    UART_Ctl[uartx].pEvent[ev]=NULL;

    switch (ev)
    {
    case UART_EV_TX_INT:
        USART_ITConfig(UART_Ctl[uartx].com,USART_IT_TC,DISABLE);
        break;
    case UART_EV_RX_INT:
        USART_ITConfig(UART_Ctl[uartx].com,USART_IT_RXNE,DISABLE);
        break;
    default:
        break;
    }
}

u8 UART_GetData(UARTx_t uartx)
{
    return USART_ReceiveData8(UART_Ctl[uartx].com);
}


void UART_SendData(UARTx_t uartx,u8* buff,u16 len)
{
    u8 byteSend,count=0;

    while (count<len)
    {
        byteSend=buff[count++];
        USART_SendData8(UART_Ctl[uartx].com,byteSend);
        while (USART_GetFlagStatus(UART_Ctl[uartx].com, USART_FLAG_TXE) == RESET);
    }
}

void UART_CommonInteruptHandle(void)
{
    for(u8 uartIndex=0; uartIndex<UART_MAX;uartIndex++)
    {
        if(USART_GetITStatus(UART_Ctl[uartIndex].com,USART_IT_RXNE)==SET)
        {
            if((UART_Ctl[uartIndex].en==ENABLE)&&(UART_Ctl[uartIndex].pEvent[UART_EV_RX_INT]!=NULL))
            {
                //USART_ClearITPendingBit(UART_Ctl[uartIndex].com, USART_IT_RXNE);
                UART_Ctl[uartIndex].pEvent[UART_EV_RX_INT]();
            }
        }               
    }
}