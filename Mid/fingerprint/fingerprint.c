#include "fingerprint.h"

#include "../common/queue.h"

#include "uart.h"
#include "timer.h"
#include "definepin.h"

static FingerPrintCmd_t             fingerPrintCmd;
u8                           fingerPrintState=STATE_IDLE;
static u32                          fingerPrintRspTimeout=0,uartRespondTimeout=0;
static FingerPrint_UserHandle_t     fingerPrintHandle=NULL;
static FingerPrint_ProcSequence_t   fingerPrintProcSeq=NULL;
static u8                           fingerPrintStep=0,isNewRspAvailable=0;
static u16                          currentEnrollUserID=0;
static u8                           errCode=ERROR_SUCCESS;
static u32                          enrollTimeout=0,searchTimeout=0;

static void FingerPrint_RevCommandHandle(void);

static void FingerPrint_SetState(u8 state);
static void FingerPrint_SetStep(u8 step);

static u16  FingerPrint_CheckSumCmd(u8 pid, u16 lengh,u8* buff);
static u16  FingerPrint_CheckSumArr(u8* buff, u16 lengh);

static u8   FingerPrint_SendCmd(u32 addr,u8 pid,u16 lengh,u8* buff);
static u8   FingerPrint_SendByteArray(u8 arr[], u16 lenght);

static u8   isProcessComplete(FingerPrintCmd_t cmd);

static void FingerPrint_HightSpeedSearch(void);
static void FingerPrint_StoreChar(u16 pageID);
static void FingerPrint_RegModel(void);
static void FingerPrint_SendGenCharBuff2(void);
static void FingerPrint_SendGenCharBuff1(void);
static void FingerPrint_SendGenImg(void);
static void FingerPrint_SendHandShakeGetInfoCmd(void);
static void FingerPrint_SendHandShakeStartCmd(void);

static void FingerPrint_ProcHandShake(void);
static void FingerPrint_ProcEnroll(void);
static void FingerPrint_ProcSearch(void);

void FingerPrint_Init(FingerPrint_UserHandle_t userHandle)
{
    GPIO_Init(GPIOC, GPIO_PIN_FINGER_CTR_POWER, GPIO_Mode_Out_PP_Low_Fast);
    UART_Init(FINGERPRINT_PORT,(u32)FINGERPRINT_BAUDRATE,UART_TX_ENABLE,UART_RX_ENABLE);
    UART_RegisterEvent(FINGERPRINT_PORT,UART_EV_RX_INT,FingerPrint_RevCommandHandle);

    UART_Enable(FINGERPRINT_PORT,ENABLE);

    fingerPrintHandle=userHandle;
}

void FingerPrint_Proc(void)
{
    switch (fingerPrintState)
    {
    case STATE_IDLE:
        /* code */
        break;
    case STATE_PROC:
        if(fingerPrintProcSeq!=NULL)
        {
            fingerPrintProcSeq();          
        }
        break;
    default:
        break;
    }
}

void FingerPrint_HandShakeCmd(void)
{
    fingerPrintProcSeq=FingerPrint_ProcHandShake;
    fingerPrintStep=HANDSHAKE_START;
    errCode=ERROR_SUCCESS;
    isNewRspAvailable=0;

    FingerPrint_SetState(STATE_PROC);
}

void FingerPrint_EnrollFingercmd(u16 userID)
{
    fingerPrintProcSeq=FingerPrint_ProcEnroll;
    fingerPrintStep=ENROLL_GET_IMG_1;
    currentEnrollUserID=userID;
    enrollTimeout=TIME_GetCurrentTime();
    isNewRspAvailable=0;
    errCode=ERROR_SUCCESS;

    FingerPrint_SetState(STATE_PROC);
}

void FingerPrint_Search(void)
{
    fingerPrintProcSeq=FingerPrint_ProcSearch;
    fingerPrintStep=SEARCH_GET_IMG;
    searchTimeout=TIME_GetCurrentTime();
    isNewRspAvailable=0;
    errCode=ERROR_SUCCESS;

    FingerPrint_SetState(STATE_PROC);
}

static u8 FingerPrint_SendCmd(u32 addr,u8 pid,u16 lengh,u8* buff)
{
    u8  header[2]={0xEF,0x01};
    u16 lengh_plus_checksum=0,checksum=0;

    lengh_plus_checksum+=2;
    checksum=FingerPrint_CheckSumCmd(pid,lengh,buff);

    UART_SendData(FINGERPRINT_PORT,header,2);
    UART_SendData(FINGERPRINT_PORT,(u8*)&addr,4);
    UART_SendData(FINGERPRINT_PORT,(u8*)&pid,1);
    UART_SendData(FINGERPRINT_PORT,(u8*)&lengh_plus_checksum,2);
    UART_SendData(FINGERPRINT_PORT,buff,lengh);
    UART_SendData(FINGERPRINT_PORT,(u8*)&checksum,2);

    return 1;
}

static u8 FingerPrint_SendByteArray(u8 arr[], u16 lenght)
{
    UART_SendData(FINGERPRINT_PORT,arr,lenght);

    return 1;
}

static void FingerPrint_RevCommandHandle(void)
{
    static u8  state=FINGERPRINT_RX_HEADER, revByte;
    static u16 revByteCount=0,lenght,checksum;
    u8  header[2]=FINGERPRINT_HEADER;
    u8*    p=(u8*)&fingerPrintCmd;


    //if(TIME_ExceedMs(uartRespondTimeout,UART_RSP_TIMEOUT))
    //{          
    //    revByteCount        = 0;
    //    state               = FINGERPRINT_RX_HEADER;
    //}

    revByte=UART_GetData(FINGERPRINT_PORT);

    //uartRespondTimeout  = TIME_GetCurrentTime();

    p[revByteCount]=revByte;

    switch (state)
    {
    case FINGERPRINT_RX_HEADER:
        if(p[revByteCount]!=header[revByteCount])
        {
            revByteCount=0;
            return;
        }else
        {
            revByteCount++;
            if(revByteCount==sizeof(header)/sizeof(u8))
            {
                state=FINGERPRINT_RX_ADDR;
            }
        }        
        break;
    case FINGERPRINT_RX_ADDR:
        revByteCount++;
        if(revByteCount==6) // header size + addr size
        {
            state=FINGERPRINT_RX_PID;
        }
        break;
    case FINGERPRINT_RX_PID:
        revByteCount++;
        state=FINGERPRINT_RX_LENGH;
        break;
    case FINGERPRINT_RX_LENGH:
        revByteCount++;
        if(revByteCount==9)
        {
           state= FINGERPRINT_RX_DATA;
        }
        break;
    case FINGERPRINT_RX_DATA:
        revByteCount++; 
        lenght=(u16)(fingerPrintCmd.lenght[0]<<8)+(u16)fingerPrintCmd.lenght[1];
        if(lenght==(revByteCount-9))
        {
            checksum=(u16)(fingerPrintCmd.buff[lenght-2]<<8)+ (u16)fingerPrintCmd.buff[lenght-1];
            if(checksum==FingerPrint_CheckSumCmd(fingerPrintCmd.pid,lenght,fingerPrintCmd.buff))
            {
                isNewRspAvailable=1;
            }
            state=FINGERPRINT_RX_HEADER;
            revByteCount=0;
        }      
        break;
    // case FINGERPRINT_RX_CHECKSUM:
    //     revByteCount++;        
    //     lenght=(u16)(fingerPrintCmd.lenght[0]<<8)+(u16)fingerPrintCmd.lenght[1];
    //     if(revByteCount<(9+lenght))
    //     {
    //     }else if(lenght >=(revByteCount-9))
    //     {
    //         checksum=(u16)(fingerPrintCmd.checksum[0]<<8)+ (u16)fingerPrintCmd.checksum[1];
    //         if(checksum==FingerPrint_CheckSumCmd(fingerPrintCmd.pid,lenght,fingerPrintCmd.buff))
    //         {
    //             isNewRspAvailable=1;
    //         }
    //         state=FINGERPRINT_RX_HEADER;
    //         revByteCount=0;
    //     }      
    //     break;
    default:
        break;
    }
}



static u16 FingerPrint_CheckSumCmd(u8 pid, u16 lengh,u8* buff)
{
    u32 checksum=0;

    checksum    =  pid;
    checksum    += (u8)(lengh>>8) + (u8)(lengh&0xff);

    for(u16 i=0 ; i<lengh-2 ; i++)
    {
        checksum+=buff[i];
    }

    return (u16)(checksum&0xffff);
}

static u16  FingerPrint_CheckSumArr(u8* buff, u16 lengh)
{
    u32 checksum=0;

    for(u16 i=0 ; i<lengh ; i++)
    {
        checksum+=buff[i];
    }

    return (u16)(checksum&0xffff);
}

static void FingerPrint_SetState(u8 state)
{
    fingerPrintState=state;
}

static void FingerPrint_SetStep(u8 step)
{ 
    fingerPrintRspTimeout    = TIME_GetCurrentTime();
    fingerPrintStep   = step;
    isNewRspAvailable =0;
}

static void FingerPrint_ProcHandShake(void)
{ 
    switch (fingerPrintStep)
    {
        case HANDSHAKE_START:
            FingerPrint_SetStep(HANDSHAKE_WAIT_START_ACK);
            FingerPrint_SendHandShakeStartCmd();
            break;
        case HANDSHAKE_WAIT_START_ACK:
            if(!isNewRspAvailable)
            {
                //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
                //{
                    errCode=ERROR_FINGER_RSP_TIMEOUT;    
                //}
            }else
            {
                if(isProcessComplete(fingerPrintCmd))
                {
                    FingerPrint_SetStep(HANDSHAKE_GET_INFO);
                }else
                {
                    errCode=fingerPrintCmd.buff[0];
                    FingerPrint_SetStep(HANDSHAKE_END);
                }           
            }            
            break;
        case HANDSHAKE_GET_INFO:
            FingerPrint_SetStep(HANDSHAKE_WAIT_INFO_ACK);
            FingerPrint_SendHandShakeGetInfoCmd();
            break;
        case HANDSHAKE_WAIT_INFO_ACK:
            if(!isNewRspAvailable)
            {
                //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
                //{
                    errCode=ERROR_FINGER_RSP_TIMEOUT;
                //    FingerPrint_SetStep(HANDSHAKE_END);
                //}
            }
            else
            {   
                if(isProcessComplete(fingerPrintCmd))
                {
                    FingerPrint_SetStep(HANDSHAKE_WAIT_INFO_DATA);
                }else
                {
                    errCode=fingerPrintCmd.buff[0];
                    FingerPrint_SetStep(HANDSHAKE_END);
                }  
            }
        case HANDSHAKE_WAIT_INFO_DATA:
            if(!isNewRspAvailable)
            {
                //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
                //{
                    errCode=ERROR_FINGER_RSP_TIMEOUT;
                 //   FingerPrint_SetStep(HANDSHAKE_END);
                //}
            }else
            {   
                if(fingerPrintCmd.pid==END_PACKET)
                {
                    FingerPrint_SetStep(HANDSHAKE_END);
                    errCode=ERROR_SUCCESS;
                }              
            }
            break;
        case HANDSHAKE_END:
            fingerPrintHandle(FINGERPRINT_HANDSHAKE_END,&errCode,1);
            FingerPrint_SetState(STATE_IDLE);
            break;
        default:
            break;
    }
}


static void FingerPrint_ProcEnroll(void)
{
    // if(TIME_ExceedMs(enrollTimeout,FINGERPRINT_ENROLL_TIMEOUT))
    // {
    //     fingerPrintStep= ENROLL_END;
    // }

    switch (fingerPrintStep)
    {
    case ENROLL_GET_IMG_1:
        FingerPrint_SendGenImg();
        FingerPrint_SetStep(ENROLL_GET_IMG_1_ACK);
        break;
    case ENROLL_GET_IMG_1_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(ENROLL_GEN_CHAR_BUFF_1);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_GET_IMG_1);
            }
        }
        break;
    case ENROLL_GEN_CHAR_BUFF_1:
        FingerPrint_SetStep(ENROLL_GEN_CHAR_BUFF_1_ACK);
        FingerPrint_SendGenCharBuff1();
        break;
    case ENROLL_GEN_CHAR_BUFF_1_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(ENROLL_GET_IMG_2);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_GET_IMG_1);
            }
        }
        break;
    case ENROLL_GET_IMG_2:
        FingerPrint_SetStep(ENROLL_GET_IMG_2_ACK);
        FingerPrint_SendGenImg();      
        break;
    case ENROLL_GET_IMG_2_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(ENROLL_GEN_CHAR_BUFF_2);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_GET_IMG_2);
            }
        }
        break;
    case ENROLL_GEN_CHAR_BUFF_2:
        FingerPrint_SetStep(ENROLL_GEN_CHAR_BUFF_2_ACK);
        FingerPrint_SendGenCharBuff2();
        break;
    case ENROLL_GEN_CHAR_BUFF_2_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(ENROLL_REG_MODEL);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_GET_IMG_2);
            }
        }
        break;
    case ENROLL_REG_MODEL:
        FingerPrint_SetStep(ENROLL_REG_MODEL_ACK);
        FingerPrint_RegModel();       
        break;
    case ENROLL_REG_MODEL_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(ENROLL_STORE);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_END);
            }
        }
        break;
    case ENROLL_STORE:
        FingerPrint_SetStep(ENROLL_STORE_ACK);
        FingerPrint_StoreChar(currentEnrollUserID);
        break;
    case ENROLL_STORE_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(ENROLL_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                errCode=ERROR_SUCCESS;
                FingerPrint_SetStep(ENROLL_END);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(ENROLL_END);
            }
        }
        break;
    case ENROLL_END:
        fingerPrintHandle(FINGERPRINT_ENROLL_END,&errCode,1);
        FingerPrint_SetState(STATE_IDLE);
        break;
    default:
        break;
    }
}

static void FingerPrint_ProcSearch(void)
{
    // if(TIME_ExceedMs(searchTimeout,FINGERPRINT_SEARCH_TIMEOUT))
    // {
    //     fingerPrintStep=SEARCH_END;
    // }

    switch (fingerPrintStep)
    {
    case SEARCH_GET_IMG:
        FingerPrint_SetStep(SEARCH_GET_IMG_ACK);
        FingerPrint_SendGenImg();       
        break;
    case SEARCH_GET_IMG_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(SEARCH_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(SEARCH_GEN_CHAR_BUFF_1);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(SEARCH_GET_IMG);
            }
        }
        break;
    case SEARCH_GEN_CHAR_BUFF_1:
        FingerPrint_SetStep(SEARCH_GEN_CHAR_BUFF_1_ACK);
        FingerPrint_SendGenCharBuff1();       
        break;
    case SEARCH_GEN_CHAR_BUFF_1_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(SEARCH_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                FingerPrint_SetStep(SEARCH_HIGH_SPEED_SEARCH);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(SEARCH_END);
            }
        }
        break;
    case SEARCH_HIGH_SPEED_SEARCH:
        FingerPrint_HightSpeedSearch();
        FingerPrint_SetStep(SEARCH_HIGH_SPEED_SEARCH_ACK);    
        break;
    case SEARCH_HIGH_SPEED_SEARCH_ACK:
        if(!isNewRspAvailable)
        {
            //if(TIME_ExceedMs(fingerPrintRspTimeout,FINGERPRINT_RSP_TIMEOUT))
            //{
                errCode=ERROR_FINGER_RSP_TIMEOUT;
            //    FingerPrint_SetStep(SEARCH_END);
            //}
        }else
        {
            if(isProcessComplete(fingerPrintCmd))
            {
                errCode=ERROR_SUCCESS;               
                FingerPrint_SetStep(SEARCH_END);
            }else
            {
                errCode=fingerPrintCmd.buff[0];
                FingerPrint_SetStep(SEARCH_END);
            }
        }
        break;
    case SEARCH_END:
        if(errCode!=ERROR_SUCCESS)
        {
            fingerPrintHandle(FINGERPRINT_SEARCH_END,&errCode,1);
        }else
        {
            
            fingerPrintHandle(FINGERPRINT_SEARCH_END,fingerPrintCmd.buff,(u16)(fingerPrintCmd.lenght[0]<<8) + (u16)(fingerPrintCmd.lenght[1])-2);
        }
                
        FingerPrint_SetState(STATE_IDLE);
        break;
    default:
        break;
    }
}

static void FingerPrint_SendHandShakeStartCmd(void)
{
    u8 data[16]={0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x07, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B};

    FingerPrint_SendByteArray(data,16);
}

static void FingerPrint_SendHandShakeGetInfoCmd(void)
{
    u8 data[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x16, 0x00, 0x1A};

    FingerPrint_SendByteArray(data,12);
}

static void FingerPrint_SendGenImg(void)
{
    u8 data[12] ={0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};

    FingerPrint_SendByteArray(data,12);
}

static void FingerPrint_SendGenCharBuff1(void)
{
    u8 data[13] ={0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, 0x01, 0x00, 0x08};

    FingerPrint_SendByteArray(data,13);
}

static void FingerPrint_SendGenCharBuff2(void)
{
    u8 data[13] ={0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, 0x02, 0x00, 0x09};

    FingerPrint_SendByteArray(data,13);
}

static void FingerPrint_RegModel(void)
{
    u8 data[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};

    FingerPrint_SendByteArray(data,12);
}

static void FingerPrint_StoreChar(u16 pageID)
{
    u16 checksum=0;
    u8  data[15] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x06, 0x06, 0x02, 0x00, 0x04, 0x00, 0x13};
  
    data[11]=(u8)(pageID >> 8);
    data[12]=(u8)(pageID & 0xff);
    
    checksum= FingerPrint_CheckSumArr(&data[6],7);
    data[13]= (u8)(checksum >>8);
    data[14]= (u8)(checksum & 0xff);

    FingerPrint_SendByteArray(data,15);
}

static void FingerPrint_HightSpeedSearch(void)
{
    u8  data[17] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x1B, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x8A}; 

    FingerPrint_SendByteArray(data,17);
}


static u8 isProcessComplete(FingerPrintCmd_t cmd)
{
    if(cmd.buff[0]==FINGERPRINT_ACK_PROCESS_COMPLETE)
    {
        return 1;
    }

    return 0;
}

