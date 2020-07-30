#ifndef _FINGERPRINT_COMMAND_H_
#define _FINGERPRINT_COMMAND_H_

#include "stm8l15x.h"

#define FINGERPRINT_ACK_PROCESS_COMPLETE            0x00
#define FINGERPRINT_ACK_REV_ERROR                   0x01
#define FINGERPRINT_ACK_NO_FINGER                   0x02
#define FINGERPRINT_ACK_FAIL_ENROLL                 0x03
#define FINGERPRINT_ACK_FAIL_GEN_DISORDELY          0x06
#define FINGERPRINT_ACK_FAIL_GEN_LACK_CHAR          0x07
#define FINGERPRINT_ACK_FINGER_NOT_MATCH            0x08
#define FINGERPRINT_ACK_FAIL_TO_FIND                0x09
#define FINGERPRINT_ACK_FAIL_TO_COMBINE             0x0A
#define FINGERPRINT_ACK_PAGEID_OVER_RANGE           0x0B
#define FINGERPRINT_ACK_TEMPLATE_ERROR              0x0C
#define FINGERPRINT_ACK_UPLOAD_TEMP_FAIL            0x0D
#define FINGERPRINT_ACK_RECEIVED_MISSING            0x0E
#define FINGERPRINT_ACK_UPLOAD_IMG_FAIL             0x0F
#define FINGERPRINT_ACK_DEL_TEMP_FAIL               0x10
#define FINGERPRINT_ACK_CLEAR_LIB_FAIL              0x11
#define FINGERPRINT_ACK_MISSING_PRIMARY_IMAGE       0x15
#define FINGERPRINT_ACK_WRITE_FLASH_ERROR           0x18
#define FINGERPRINT_ACK_UNKOWN_ERROR                0x19
#define FINGERPRINT_ACK_INVALID_REG_NB              0x1A
#define FINGERPRINT_ACK_INCORRECT_CFG_REG           0x1B
#define FINGERPRINT_ACK_WRONG_NOTEPAD_PAGE          0x1C
#define FINGERPRINT_ACK_SERIAL_PORT_FAIL            0x1D
#define FINGERPRINT_ACK_RESERVER                    0x1E

enum
{
    FINGERPRINT_GET_HANDSHAKE_INFO,
    FINGERPRINT_HANDSHAKE_END,
    FINGERPRINT_ENROLL_END,
    FINGERPRINT_SEARCH_END,
};

typedef void (*FingerPrint_UserHandle_t)(u8 event, u8* par, u8 par_len);

void FingerPrint_AppHandle(u8 event, u8* par, u8 par_len);


#endif