#include "debug.h"
#include "uart.h"

#include "stdio.h"



#define PUTCHAR_PROTOTYPE int putchar(int c)

PUTCHAR_PROTOTYPE
{
    UART_SendData(DBG_PORT,(u8*)&c,1);
    return c;
}

void DBG_Init(void)
{
    UART_Init(DBG_PORT,57600,UART_TX_ENABLE,UART_RX_DISABLE);
    UART_Enable(DBG_PORT,ENABLE);
}


void DBG_PrintString(char* str)
{
    printf("%s\n",str);
}

void DBG_PrintHex(u8 *source,u8 len)
{
    for(int i=0;i<len;i++)
    {
        printf("%02x",source[i]);
        printf(" ");
    }
}

