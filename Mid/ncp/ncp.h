#ifndef _NCP_H_
#define _NCP_H_

#include "uart.h"

#define NCP_COMMAND_QUEUE_SIZE  3
#define NCP_COMMAND_MAX_LENGH   50

#define NCP_COMMAND_HEADER_SIZE 4


typedef struct NCP_Command_t
{
    u8 command;
    u8 type;
    u8 sequence;
    u8 lengh;
    u8 buff[NCP_COMMAND_MAX_LENGH];
}NCP_Command_t;

#define NCP_PORT        UART2
#define NCP_BAUDRATE    19200

void NCP_Init(void);
void NCP_Proc(void);
void NCP_Send(u8* buff, u8 lenght);


#endif