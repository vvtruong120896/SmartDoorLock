#include "ncp.h"
#include "timer.h"
#include "stddef.h"

#include "../common/queue.h"
#include "../common/xor.h"

#include "../../App/zb_command.h"

static void     NCP_RevCommandHandle(void);


NCP_Command_t ncpCommandBuff[NCP_COMMAND_QUEUE_SIZE];
QUEUEx_t      ncpComandQueue;

static u32   ncpUARTTimeout=0;

void NCP_Init(void)
{   
    UART_Init(NCP_PORT,(u32)NCP_BAUDRATE,UART_TX_ENABLE,UART_RX_ENABLE);
    UART_RegisterEvent(NCP_PORT,UART_EV_RX_INT,NCP_RevCommandHandle);
    
    QUEUE_Init(&ncpComandQueue,(u8*)ncpCommandBuff,NCP_COMMAND_QUEUE_SIZE,sizeof(NCP_Command_t));

    UART_Enable(NCP_PORT,ENABLE);
}

static void NCP_RevCommandHandle(void)
{
    static u8 revByteCount=0;
    static u8 revBuff[NCP_COMMAND_HEADER_SIZE+NCP_COMMAND_MAX_LENGH+1];
    u8        revByte=0;
   
    //if(TIME_ExceedMs(ncpUARTTimeout,UART_RSP_TIMEOUT))
    //{          
    //    revByteCount        = 0;
    //}
    
    revByte=UART_GetData(NCP_PORT);
    
    //ncpUARTTimeout  = TIME_GetCurrentTime();

    revBuff[revByteCount++]=revByte;
    if(revByteCount>=NCP_COMMAND_HEADER_SIZE)
    {
        if(revByteCount==(NCP_COMMAND_HEADER_SIZE+revBuff[3] +1))
        {
            if(revBuff[NCP_COMMAND_HEADER_SIZE+revBuff[3]]==XOR_Caculator(revBuff,0,NCP_COMMAND_HEADER_SIZE+revBuff[3]))
            {
                QUEUE_Push(&ncpComandQueue,revBuff);
            }
                       
            revByteCount=0;
        }
    }
}

void NCP_Proc(void)
{
    NCP_Command_t ncpCommand;

    if(!QUEUE_Empty(&ncpComandQueue))
    {
        QUEUE_Get(&ncpComandQueue,(u8*)&ncpCommand);

        ZB_CheckCommandExitsAndExecute(ncpCommand.command,ncpCommand.type,ncpCommand.buff,ncpCommand.lengh);
    }
}

void NCP_Send(u8* buff, u8 lenght)
{
    UART_SendData(NCP_PORT,buff,(u16)lenght);
}

