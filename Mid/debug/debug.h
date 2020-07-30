#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "uart.h"

#define DBG_PORT    UART2

void DBG_Init(void);
void DBG_PrintString(char* str);
void DBG_PrintHex(u8 *source,u8 len);




#endif