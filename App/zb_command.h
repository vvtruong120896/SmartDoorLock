#ifndef _ZB_COMMAND_H_
#define _ZB_COMMAND_H_


#include "stm8l15x.h"

#define DEVICE_ANNOUNCE                         0x44
#define SEND_FIRST_TIME                         0x19
#define SEND_FIRST_TIME_1                       0x1C
#define SEND_FIRST_TIME_2                       0x1B

#define DEVICE_ANNOUNCE                         0x44
#define SEND_FIRST_TIME                         0x19
#define SEND_FIRST_TIME_1                       0x1C
#define SEND_FIRST_TIME_2                       0x1B

/* Command type */
#define ZB_REQUEST_TYPE                         0xA0
#define ZB_RESPONSE_TYPE                        0xA1
#define YL_UPDATE_TYPE                          0xB0
#define YL_RESPONSE_TYPE                        0xB1
#define YL_ERROR_TYPE                           0xB2

#define MAX_COMMAND_ID_1                        0x80
#define MIN_COMMAND_ID_1                        0x00
#define HIGH_4_COMMAND_ID_2                     0xB0

/* Network */
#define JOIN_NETWORK_RESULT_CMD                 0x21
#define TOGGLE_NETWORK_CMD                      0x36
#define GET_NETWORK_INFO_CMD                    0x37

#define NETWORK_SUCCESS_RES                     0x00
#define NETWORK_FAILURE_RES                     0xFF

/* Lock/ Unlock */
#define CHANGE_LOCK_STATE_CMD                   0x30
#define FINGER_PRINT_LOCK_CMD                   0x57
#define SET_LOCK_CMD                            0x10
#define GET_LOCK_CMD                            0x11

#define LOCK_SETUP_STATE                        0xFF
#define UNLOCK_SETUP_STATE                      0x00

#define UNLOCK_BY_PHYSICAL                      0x16
#define UNLOCK_BY_ZIGBEE                        0x19
#define UNLOCK_BY_PIN                           0x13
#define UNLOCK_BY_FINGER_PRINT                  0x02
#define UNLOCK_BY_GET_STATE                     0x00
#define WARNING_UNLOCK                          0xA1

#define LOCK_BY_PHYSICAL                        0x15
#define LOCK_BY_ZIGBEE                          0x18
#define LOCK_BY_AUTO                            0x1B
#define LOCK_BY_GET_STATE                       0x21

#define PIN_CODE_TYPE                           0x00
#define FINGERPRINT_TYPE                        0x02

/* Battery */
#define GET_BATTERY_CMD                         0x1A
#define WARNING_LOW_BATTERY                     0xA7

/* Language */
#define CHANGE_LANGUAGE_CMD                     0x18
#define UPDATE_LANGUAGE_CMD                     0x33

/* Finger print */
#define SET_FINGERPRINT_CMD                     0x74
#define CLEAR_FINGERPRINT_CMD                   0x51
#define CLEAR_FINGERPRINT_RES                   0x53
#define UPDATE_FINGERPRINT_CMD                  0x52

/* User code */
#define GET_PIN_CODE_CMD                        0x15
#define SET_PIN_CODE_CMD                        0x12
#define CLEAR_PIN_CODE_CMD                      0x13
#define GET_ALL_PIN_CODE_CMD                    SET_FINGERPRINT_CMD

#define UPDATE_PIN_CODE_CMD                     0x31
#define CLEAR_PIN_CODE_RES                      0x32

#define CHANGE_MASTER_CODE_CMD                  0x70
#define MASTER_CODE_USER_ID                     0xFB

#define SET_USER_CODE_MULTIPLE                  0x71

/* Log state */
#define IS_LOCKED_STATE                         0x01
#define IS_UNLOCKED_STATE                       0x02

#define SET_PIN_STATE                           0x03
#define DELETE_PIN_STATE                        0x04
#define SET_FINGERPRINT_STATE                   0x05
#define DELETE_FINGERPRINT_STATE                0x06

#define WARNING_UNLOCK_ILLEGAL_STATE            0x07
#define WARNING_LOW_BATTERY_STATE               0x08

#define ERROR_SET_PIN_CODE                      0x13
#define ERROR_SET_FP_FULL                       0x15

#define MAX_USER_CODE_SIZE                      10
#define MIN_USER_CODE_SIZE                      4

#define MAX_USER_ID                             30
#define MIN_USER_ID                             1


#define USE_TEST_FINGER_PRINT                   0xFF

typedef void (*ZB_CommandHandle_t)(u8 type,u8* buff,u8 len);

typedef struct ZB_CommandInfo_t
{
    u8 cmdId;
    ZB_CommandHandle_t cmdHandle;
}ZB_CommandInfo_t;


// User command
void ZB_CommandRespond(u8 command, u8 type,u8* buff, u8 len);


// stack command, user not use
void ZB_CheckCommandExitsAndExecute(u8 command, u8 type,u8* buff, u8 len);



void ZB_Wakeup(void);


#endif