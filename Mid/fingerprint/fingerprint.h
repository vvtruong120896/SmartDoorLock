#ifndef _FINGERPRINT_H_
#define _FINGERPRINT_H_

#include "stm8l15x.h"

#include "../../App/fingerprint_app.h"
#define FINGERPRINT_BUFF_MAX    256
#define FINGERPRINT_QUEUE_SIZE  3

#define FINGERPRINT_PORT        UART1
#define FINGERPRINT_BAUDRATE    57600

#define FINGERPRINT_HEADER      {0xEF,0x01}

#define FINGERPRINT_RSP_TIMEOUT                 100


#define FINGERPRINT_ENROLL_TIMEOUT              5000
#define FINGERPRINT_SEARCH_TIMEOUT              5000

enum
{
    CMD_PACKET  =0x01,
    DATA_PACKET =0x02,
    ACK_PACKET  =0x07,
    END_PACKET  =0x08
};


#define FINGERPRINT_CMD_GET_IMG                     0x01
#define FINGERPRINT_CMD_GEN_CHAR                    0x02
#define FINGERPRINT_CMD_MATCH                       0x03
#define FINGERPRINT_CMD_SEARCH                      0x04
#define FINGERPRINT_CMD_REG_MODEL                   0x05
#define FINGERPRINT_CMD_STORE_CHAR                  0x06
#define FINGERPRINT_CMD_READ_TMP                    0x07
#define FINGERPRINT_CMD_UP_CHAR                     0x08
#define FINGERPRINT_CMD_DOWN_CHAR                   0x09
#define FINGERPRINT_CMD_UP_IMG                      0x0A
#define FINGERPRINT_CMD_DOWN_IMG                    0x0B
#define FINGERPRINT_CMD_DEL_CHAR                    0x0C
#define FINGERPRINT_CMD_EMPTY                       0x0D
#define FINGERPRINT_CMD_SET_SYS_PARAM               0x0E
#define FINGERPRINT_CMD_GET_SYS_PARAM               0x0F
#define FINGERPRINT_CMD_GEN_RAND_CODE               0X14
#define FINGERPRINT_CMD_SET_ADDRESS                 0x15
#define FINGERPRINT_CMD_COM_LINK                    0x17
#define FINGERPRINT_CMD_WRITE_NOTEPAD               0X18
#define FINGERPRINT_CMD_READ_NOTEPAD                0X19
#define FINGERPRINT_CMD_HIGHT_SPEED_SEARCH          0x1B
#define FINGERPRINT_CMD_HIGHT_VALID_TEMP_NUM        0x1D
#define FINGERPRINT_CMD_NONE                        0xFF

enum
{
    FINGERPRINT_RX_HEADER,
    FINGERPRINT_RX_ADDR,
    FINGERPRINT_RX_PID,
    FINGERPRINT_RX_LENGH,
    FINGERPRINT_RX_DATA,
};


enum
{
    STATE_IDLE,
    STATE_PROC,
};


enum
{
    HANDSHAKE_START,
    HANDSHAKE_WAIT_START_ACK,
    HANDSHAKE_GET_INFO,
    HANDSHAKE_WAIT_INFO_ACK,
    HANDSHAKE_WAIT_INFO_DATA,
    HANDSHAKE_END
};


enum
{
    ENROLL_GET_IMG_1,
    ENROLL_GET_IMG_1_ACK,
    ENROLL_GEN_CHAR_BUFF_1,
    ENROLL_GEN_CHAR_BUFF_1_ACK,
    ENROLL_GET_IMG_2,
    ENROLL_GET_IMG_2_ACK,
    ENROLL_GEN_CHAR_BUFF_2,
    ENROLL_GEN_CHAR_BUFF_2_ACK,
    ENROLL_REG_MODEL,
    ENROLL_REG_MODEL_ACK,
    ENROLL_STORE,
    ENROLL_STORE_ACK,
    ENROLL_END,
};


enum
{
    SEARCH_GET_IMG,
    SEARCH_GET_IMG_ACK,
    SEARCH_GEN_CHAR_BUFF_1,
    SEARCH_GEN_CHAR_BUFF_1_ACK,
    SEARCH_HIGH_SPEED_SEARCH,
    SEARCH_HIGH_SPEED_SEARCH_ACK,
    SEARCH_END,
};

enum
{
    ERROR_SUCCESS,
    ERROR_FINGER_RSP_TIMEOUT=0xFF,
};




typedef void (*FingerPrint_ProcSequence_t)(void);


typedef struct FingerPrintCmd_t
{
    u8  header[2];
    u8  addr[4];
    u8  pid;
    u8  lenght[2];
    u8  buff[FINGERPRINT_BUFF_MAX];     
}FingerPrintCmd_t;


void FingerPrint_Init(FingerPrint_UserHandle_t userHandle);
void FingerPrint_Proc(void);

void FingerPrint_HandShakeCmd(void);
void FingerPrint_EnrollFingercmd(u16 userID);
void FingerPrint_Search(void);

#endif