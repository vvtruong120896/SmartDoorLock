#include "mainfunctionlist.h"
#include "stm8l15x_it.h"
#include "clock.h"
#include "define.h"
#include "definepin.h"
#include "managerpower.h"
#include "delay.h"
#include "Softwaretimer.h"
#include "drivermotor.h"
#include "eeprom.h"
#include "defaultdevicecode.h"
#include "exti.h"
#include "getpassword.h"
#include "leddisplay.h"
#include "sleep.h"
#include "tsm12.h"
#include "audio.h"
#include "uart.h"
#include "ncp.h"
#include "zb_command.h"
#include "fingerprint_app.h"
#include "fingerprint.h" 
#include "timer.h"
#include "mfcv520.h"
#include <stddef.h>
#include <string.h>


/* Private variables -------------------------------------------*/
bool Check_Status_Success = true, Check_Byte_In_Eeprom = FALSE, Check_Password = FALSE;
bool Check_Type_Password = FALSE, Check_Password_Enroll = TRUE, Check_Card_Enroll = TRUE;
bool Check_Card = FALSE;
uint8_t Password_Array[16], Password_Save[16], ID_Clear[5];
uint8_t Check_Key, Check_Number_Of_Array, Type_Setting, Check_Language;
uint8_t Temp_ID = 1, Key = 0, ID_Value = 0, Check_Password_False = 0;
uint8_t Numberofpassword = 0, Check_Id = 0, Use_Left_Lock_State = 0;
uint16_t Temp_ADDRESS = 16;
volatile uint8_t Check_Motor;
volatile uint8_t Finger_FlagError = 0, DistanceTwiceReadCard = 0, DistanceTwiceReadFingerprint = 0;

extern NCP_Command_t ncpCommandBuff[NCP_COMMAND_QUEUE_SIZE];
extern volatile uint8_t zb_flagnetworkinfo, networkstatus, Enter_Freezen;
extern volatile uint8_t Button_Card_Flag, Finger_Flag, Bell_Flag, zb_flagsetlock, Switch_Flag;
extern volatile uint8_t Timeout_Flag, Sleep_Flag, Fingerflag, Temper_Flag;
extern volatile uint8_t zb_flagtogglenetwork, zb_flagchangelockstate, zb_flagjoinnetwork;
extern uint8_t Buffer_vtupdatenetworkinfo[5];
extern uint8_t Buffer_vtupdatenetwork[5];
extern uint8_t Buffer_vtupdatelock[6];
extern uint8_t Buffer_vtupdateunlock[6];
extern uint8_t Buffer_vtupdatewarningunlock[6];
extern volatile uint16_t Timeout_Sound_Count, Start_Count;


void Function_Check_All_Flag(void)
{
    if((Button_Card_Flag == 1) || (Finger_Flag == 1) || (zb_flagsetlock == 1) ||\
       (Bell_Flag == 1) || (Switch_Flag == 1))  
    {
        //Check All Flag
        if (Button_Card_Flag == 1) 
        {   
            // // Check Keypad and Card
            if(Function_Check_Card() != TRUE)
            {
                Function_Check_Button();
            }  

        }
        else if(Finger_Flag == 1)
        {   
            // Check Fingerprint
            Function_Check_Fingerprint();
        }
        else if(zb_flagsetlock == 1)
        {
            // Check Zigbee
            AUDIO_Handle_Send(VERIFY_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            LED_Control_Power_On(); 
            ZB_Wakeup();
            NCP_Send(Buffer_vtupdateunlock, sizeof(Buffer_vtupdateunlock));
            Drivermotor_Open(); Check_Motor = 1;
            zb_flagsetlock = 0;
        }
        else if(Bell_Flag == 1)
        {
            // Check Bell
            LED_Control_Power_On(); 
            LED_Dispaly(0x40, 0x00); 
            AUDIO_Handle_Send(SOUND_WHEN_PRESS_BELL, 0, 0, MID_MIN_TIME_SOUND);
            Sleep_Flag = 1;
        }
        else if(Switch_Flag == 1)
        {
            // Check Switch
            delay_ms(1000);
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_PIN_SWITCH_INT) == 0)
            {
                LED_Control_Power_On(); 
                AUDIO_Handle_Send(SOUND_WHEN_TURN_SWITCH, 0, 0, MIN_TIME_SOUND);
                ZB_Wakeup(); 
                NCP_Send(Buffer_vtupdatelock, sizeof(Buffer_vtupdateunlock));
            }
            else if(GPIO_ReadInputDataBit(GPIOE, GPIO_PIN_RESET_INT) == 0)
            {
                LED_Control_Power_On();
                AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
                delay_ms(300);
                memset(Password_Array, 0xFF, SIZEOFFRAME);
                for(uint16_t i = 0; i < ALL_FRAME_USE_COMMON; i++)
                {
                    WRITER_Frame_To_Eeprom(ADDRESS_KEY_FIRST+16*i, Password_Array, SIZEOFFRAME);
                }
                AUDIO_Send(BIG_SOUND); // Default sound
                Check_Language = 0; // Vietnamese
                AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_MIN_TIME_SOUND);
            }
            Sleep_Flag = 1;
        }
    }

    if(Temper_Flag == 1)
    {
        LED_Control_Power_On();
        ZB_Wakeup();
        NCP_Send(Buffer_vtupdatewarningunlock, sizeof(Buffer_vtupdatewarningunlock));
        if(GPIO_ReadInputDataBit(GPIOG, GPIO_PIN_TEMPER_INT) == 0)
        {
            disableInterrupts();
            while (1)
            {
                AUDIO_Send(SOUND_WHEN_WANING);
                delay_ms(10000);
            }
        }
    }
}

bool Function_Check_Card(void)
{
    DistanceTwiceReadCard++;
    if(DistanceTwiceReadCard == 1)
    {
        CARD_Control_Power_On();
        memset(Password_Array, 0, sizeof(Password_Array));
        READ_Card(Password_Array);
        Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao
        Check_Byte_In_Eeprom = Function_Check_AllByte_Use_In_Eeprom();
        if(Password_Array[0] == NULL)
        {
            return FALSE;
        }
        else
        {
            if(Check_Byte_In_Eeprom == FALSE)
            {
                // eeprom khong co data
                Function_Save_Card_Into_Eeprom(Password_Array);
                return TRUE;
            }
            else
            {
                // eeprom co data
                Check_Card = Function_Handle_Card(Password_Array);
                if(Check_Card == TRUE)
                {   
                    // Card chinh xac
                    Function_Handle_Card_Correct();
                    Button_Card_Flag = 0;
                    return TRUE;

                }
                else
                {
                    // Card khong chinh xac
                    Function_Handle_Card_Not_Correct();
                    return TRUE;
                }   
            }
        }
        
    }
    return FALSE;
}

void Function_Save_Card_Into_Eeprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            LED_Control_Power_On();
            Password_Array[i] = ID; Password_Array[i+1] = MANAGER; Password_Array[i+2] = END_FRAME;
            WRITER_Frame_To_Eeprom(ADDRESS_CARD_FIRST, Password_Array, Check_Number_Of_Array+3); // Luu password vao eeprom
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

bool Function_Handle_Card(uint8_t* Password_Array)
{
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        READ_Frame_From_Eeprom(Password_Save, ADDRESS_CARD_FIRST+16*i, SIZEOFFRAME);
        if(memcmp(Password_Array, Password_Save, 4) == 0)
        {
            Check_Id = Password_Save[4];
            memset(Password_Array, NULL, SIZEOFFRAME);
            return TRUE;
        }
    }
    return FALSE;
}

void Function_Handle_Card_Correct(void)
{
    if(Check_Language == 0)
    {
        AUDIO_Send(VERIFY_SUCCESSFUL);
    }
    else
    {
        AUDIO_Send(VERIFY_SUCCESSFUL+1);
    }
    ZB_Wakeup();
    NCP_Send(Buffer_vtupdateunlock, sizeof(Buffer_vtupdateunlock));
    Check_Password_False = 0;
    Check_Type_Password = Function_Check_Card_Manager_Or_User(Check_Number_Of_Array);
    if(Check_Type_Password == TRUE)
    {
        // Card is manager
        Drivermotor_Open(); Check_Motor = 1;
        Function_Handle_Card_Manager();
    }
    else
    {
        // Card is user
        if(Use_Left_Lock_State == 0)
        {
            Drivermotor_Open(); Check_Motor = 1;
        }
        Function_Handle_Card_User();
    }
}

bool Function_Check_Card_Manager_Or_User(uint8_t Check_Number_Of_Array)
{
    if(Password_Save[Check_Number_Of_Array + 1] == MANAGER) // Kiem tra co phai la Manager hay khong
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void Function_Handle_Card_Manager(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat led * #
    do
    {
        Key = GET_Key();
        if(Key == '*')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Notification_Id();
            Sleep_Flag = 1;
        }
        else if(Key == '#')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting();
            break;
        }
    } while ((Key != '*') && (Sleep_Flag == 0) && (Key != '#'));
    
}

void Function_Handle_Card_User(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x02, 0x00); // Bat led *
    do
    {
        Key = GET_Key();
        if(Key == '*')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Notification_Id();
            Sleep_Flag = 1;
        }

    } while ((Key != '*') && (Sleep_Flag == 0));
    
}

void Function_Handle_Card_Not_Correct(void)
{
    Check_Password_False++;
    if(Check_Password_False == 3)
    {
        Check_Password_False = 0;
        Enter_Freezen = 1;
        ZB_Wakeup();
        NCP_Send(Buffer_vtupdatewarningunlock, sizeof(Buffer_vtupdatewarningunlock));
        LED_Dispaly(0x00, 0x00);
        AUDIO_Handle_Send(KEYBOARD_IS_LOCKED, 0, 0, NORMAL_TIME_SOUND);
        Button_Card_Flag = 0;
        LED_Control_Power_Off();
        while((Enter_Freezen == 1) && (Temper_Flag == 0))
        {
            Button_Card_Flag = 0;
            // cho het 5p
        }
        if(Enter_Freezen == 0)
        {
            LED_Control_Power_On();
            delay_ms(300);
            Sleep_Flag = 1;
        }
        else
        {
            Sleep_Flag = 0;
        }
        
    }
    else
    {
        LED_Control_Power_On();
        if(Check_Language == 0)
        {
            AUDIO_Send(VERIFY_NOT_SUCCESSFUL);
        }
        else
        {
            AUDIO_Send(VERIFY_NOT_SUCCESSFUL+1);
        }
        delay_ms(5000);
        Button_Card_Flag = 0;
        Sleep_Flag = 1;
    }
}

void Function_Check_Button(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    delay_ms(1000);
    LED_Dispaly(0x0F, 0xFF); // Bat tat ca led ban phim
    delay_ms(10);
    AUDIO_Send(SOUND_WHEN_TOUCHING_KEYBOARD_FIRST_TIME);
    delay_ms(500);
    Check_Key = GET_Password(Password_Array); // Doc key
    Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao

    if(Check_Key == REPEAT)// Kiem tra xem # co duoc nhan hay khong
    {
        Check_Byte_In_Eeprom = Function_Check_AllByte_Use_In_Eeprom();

        if (Check_Byte_In_Eeprom == FALSE)// eeprom khong co data
        {
            Function_Save_Password_into_Eepprom(Password_Array);
        }
        else// eeprom co data
        {
            Check_Password = Function_Check_Password(Password_Array); // So sanh password
            if(Check_Password == TRUE)// password chinh xac
            {
                Function_Handle_Password_Correct();
            }
            else// Password khong chinh xac
            {
                Function_Handle_Password_Not_Correct();
            }       
        }
    }
    else if(Check_Key == BACK)  // Kiem tra xem * co duoc nhan hay khong
    {
        LED_Dispaly(0x00, 0x00); // Bat tat ca led ban phim
        Sleep_Flag = 1; // vao sleep mode
    }
    Button_Card_Flag = 0;       
}

uint8_t Function_Check_Number_Of_Array(uint8_t* array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++) // Tim so phan tu co trong mang
    {
        if(array[i] == NULL)
        {
            return i;
        }
    }
    return 0;
}

bool Function_Check_AllByte_Use_In_Eeprom(void)
{
    static uint8_t Check_Eeprom;
    for(uint16_t i = 0; i < ALL_BYTE_USE-240; i++)// Check all byte use in eeprom 
    {
        Check_Eeprom = READ_Byte_From_Eeprom(ADDRESS_KEY_FIRST+i);
        if(Check_Eeprom != 0xFF) // eeprom com data
        {
            return TRUE;
        }
        delay_ms(5);
    }
    for(uint16_t i = 0; i < ALL_BYTE_USE-240; i++)// Check all byte use in eeprom 
    {
        Check_Eeprom = READ_Byte_From_Eeprom(ADDRESS_CARD_FIRST+i);
        if(Check_Eeprom != 0xFF) // eeprom com data
        {
            return TRUE;
        }
        delay_ms(5);
    }
    for(uint16_t i = 0; i < ALL_BYTE_USE-240; i++)// Check all byte use in eeprom 
    {
        Check_Eeprom = READ_Byte_From_Eeprom(ADDRESS_FINGERPRINT_FIRST+i);
        if(Check_Eeprom != 0xFF) // eeprom com data
        {
            return TRUE;
        }
        delay_ms(5);
    }
    return FALSE; 
}

void Function_Save_Password_into_Eepprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            Password_Array[i] = ID; Password_Array[i+1] = MANAGER; Password_Array[i+2] = END_FRAME;
            WRITER_Frame_To_Eeprom(ADDRESS_KEY_FIRST, Password_Array, Check_Number_Of_Array+3); // Luu password vao eeprom
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

bool Function_Check_Password(uint8_t* Password_Array)
{
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        Numberofpassword = READ_Frame_From_Eeprom(Password_Save, ADDRESS_KEY_FIRST+16*i, SIZEOFFRAME);
        if(Function_Compare_Password(Password_Save, Password_Array))
        {
            Check_Id = Password_Save[Numberofpassword];
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            return TRUE;
        }
    }
    return FALSE;
    
}

bool Function_Compare_Password(uint8_t *Password_Save, uint8_t *Password_Enter)
{
    uint8_t Count = 0, Temp = 0;
	uint8_t Sizeofpasswordsave = Numberofpassword;
	uint8_t Sizeofpasswordenter = Check_Number_Of_Array;
	for (uint8_t i = 0; i < Sizeofpasswordenter; i++)
	{
		if (Password_Enter[i] == Password_Save[Temp])
		{
			Count++; Temp = Temp + 1;
			if(Count == Sizeofpasswordsave)
			{
				return TRUE;
			}
		}
		else
		{
			Count = 0; Temp = 0; 
		}
	}
	return FALSE;
}


bool Function_Check_Password_Manager_Or_User(uint8_t Check_Number_Of_Array)
{
    if(Password_Save[Numberofpassword + 1] == MANAGER) // Kiem tra co phai la Manager hay khong
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}

void Function_Handle_Password_Not_Correct(void)
{
    Check_Password_False++;
    if(Check_Password_False == 3)
    {
        Check_Password_False = 0;
        Enter_Freezen = 1;
        ZB_Wakeup();
        NCP_Send(Buffer_vtupdatewarningunlock, sizeof(Buffer_vtupdatewarningunlock));
        LED_Dispaly(0x00, 0x00);
        AUDIO_Handle_Send(KEYBOARD_IS_LOCKED, 0, 0, NORMAL_TIME_SOUND);
        LED_Control_Power_Off();
        while((Enter_Freezen == 1) && (Temper_Flag == 0))
        {
            // cho het 5p
        }
        if(Enter_Freezen == 0)
        {
            LED_Control_Power_On();
            delay_ms(300);
            Sleep_Flag = 1;
        }
        else
        {
            Sleep_Flag = 0;
        }
        
    }
    else
    {
        if(Check_Language == 0)
        {
            AUDIO_Send(VERIFY_NOT_SUCCESSFUL);
        }
        else
        {
            AUDIO_Send(VERIFY_NOT_SUCCESSFUL+1);
        }
        Check_Status_Success = false;
        if(Check_Status_Success == true)
        {
            Sleep_Flag = 1;
        }
    }
}

void Function_Handle_Password_Correct(void)
{
    if(Check_Language == 0)
    {
        AUDIO_Send(VERIFY_SUCCESSFUL);
    }
    else
    {
        AUDIO_Send(VERIFY_SUCCESSFUL+1);
    }
    
    ZB_Wakeup();
    NCP_Send(Buffer_vtupdateunlock, sizeof(Buffer_vtupdateunlock));
    Check_Password_False = 0;
    Check_Type_Password = Function_Check_Password_Manager_Or_User(Check_Number_Of_Array);
    if(Check_Type_Password == TRUE)
    {
        // Password is manager
        Drivermotor_Open(); Check_Motor = 1;
        Function_Handle_Password_Manager();
    }
    else
    {
        // Password is user
        if(Use_Left_Lock_State == 0)
        {
            Drivermotor_Open(); Check_Motor = 1;
        }
        Function_Handle_Password_User();
    }
    
        
}

void Function_Handle_Password_User(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x02, 0x00); // Bat led *
    do
    {
        Key = GET_Key();
        if(Key == '*')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Notification_Id();
            Sleep_Flag = 1;
        }

    } while ((Key != '*') && (Sleep_Flag == 0));
    
}

void Function_Notification_Id(void)
{
    uint8_t temp;
    uint8_t odr1 = 0, odr2 = 0;
    temp = Check_Id;
    odr1 = temp/10;
    odr2 = temp%10;
    AUDIO_Handle_Send(USER_ORDER_NUMBER, 0, 0, MID_MIN_TIME_SOUND);

    switch(odr1)
    {
    case 0:
        AUDIO_Handle_Send(SPEAK_0, 0, 0, MIN_TIME_SOUND);
        break;
    case 1:
        AUDIO_Handle_Send(SPEAK_1, 0, 0, MIN_TIME_SOUND);
        break;
    case 2:
        AUDIO_Handle_Send(SPEAK_2, 0, 0, MIN_TIME_SOUND);
        break;
    case 3:
        AUDIO_Handle_Send(SPEAK_3, 0, 0, MIN_TIME_SOUND);
        break;
    case 4:
        AUDIO_Handle_Send(SPEAK_4, 0, 0, MIN_TIME_SOUND);
        break;
    case 5:
        AUDIO_Handle_Send(SPEAK_5, 0, 0, MIN_TIME_SOUND);
        break;
    case 6:
        AUDIO_Handle_Send(SPEAK_6, 0, 0, MIN_TIME_SOUND);
        break;
    case 7:
        AUDIO_Handle_Send(SPEAK_7, 0, 0, MIN_TIME_SOUND);
        break;
    case 8:
        AUDIO_Handle_Send(SPEAK_8, 0, 0, MIN_TIME_SOUND);
        break;
    case 9:
        AUDIO_Handle_Send(SPEAK_9, 0, 0, MIN_TIME_SOUND);
        break;
    default:
        break;
    }

    switch(odr2)
    {
    case 0:
        AUDIO_Handle_Send(SPEAK_0, 0, 0, MIN_TIME_SOUND);
        break;
    case 1:
        AUDIO_Handle_Send(SPEAK_1, 0, 0, MIN_TIME_SOUND);
        break;
    case 2:
        AUDIO_Handle_Send(SPEAK_2, 0, 0, MIN_TIME_SOUND);
        break;
    case 3:
        AUDIO_Handle_Send(SPEAK_3, 0, 0, MIN_TIME_SOUND);
        break;
    case 4:
        AUDIO_Handle_Send(SPEAK_4, 0, 0, MIN_TIME_SOUND);
        break;
    case 5:
        AUDIO_Handle_Send(SPEAK_5, 0, 0, MIN_TIME_SOUND);
        break;
    case 6:
        AUDIO_Handle_Send(SPEAK_6, 0, 0, MIN_TIME_SOUND);
        break;
    case 7:
        AUDIO_Handle_Send(SPEAK_7, 0, 0, MIN_TIME_SOUND);
        break;
    case 8:
        AUDIO_Handle_Send(SPEAK_8, 0, 0, MIN_TIME_SOUND);
        break;
    case 9:
        AUDIO_Handle_Send(SPEAK_9, 0, 0, MIN_TIME_SOUND);
        break;
    default:
        break;
    }
    
}

void Function_Handle_Password_Manager(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat led * #
    do
    {
        Key = GET_Key();
        if(Key == '*')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Notification_Id();
            Sleep_Flag = 1;
        }
        else if(Key == '#')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting();
            break;
        }

    } while ((Key != '*') && (Key != '#') && (Sleep_Flag == 0));
        
}

uint8_t Function_Handle_Setting(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x1F); // Bat led * #, 1, 2, 3, 4, 5
    do
    {
        Function_Handle_Package_Sound_Step1();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            LED_Dispaly(0x00, 0x00);
            Sleep_Flag = 1;
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Resgister();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Clear();
            break;
        case '3':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_System();
            break;
        case '4':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Introduce();
            break;
        case '5':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Join_Network();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while((Key == '#') && (Sleep_Flag == 0));
    return 0;
}

void Function_Handle_Package_Sound_Step1(void)
{
    do
    {
        Key = AUDIO_Handle_Send(REGISTER_USER, PRESS_KEY, SPEAK_1, MID_MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(DELETE_USER, PRESS_KEY, SPEAK_2, MID_MIN_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(INSTALL_SYSTEM, PRESS_KEY, SPEAK_3, MID_MIN_MIN_TIME_SOUND);
                if(Key != 0) break;
                else
                {
                    if(Check_Language == 0)
                    {
                        Key = AUDIO_Handle_Send(INTRUDUCTION_EQUIPMENT, PRESS_KEY, SPEAK_4, NORMAL_TIME_SOUND);
                    }
                    else
                    {
                        Key = AUDIO_Handle_Send(INTRUDUCTION_EQUIPMENT, PRESS_KEY, SPEAK_4, MID_TIME_SOUND);
                    }
                    
                    if(Key != 0) break;
                    else
                    {
                        Key = AUDIO_Handle_Send(SETTING_NETWORK, PRESS_KEY, SPEAK_5, NORMAL_TIME_SOUND);
                        
                        if(Key != 0) break;
                        else
                        {
                            Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                            if(Key != 0) 
                            {
                                Start_Count = 0;
                                break;
                            }
                            else
                            {
                                Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0,  MID_TIME_SOUND);
                                if(Key != 0) 
                                {
                                    Start_Count = 0;
                                    break;
                                }
                                else
                                {
                                    Start_Count = 0;
                                    break;
                                }
                                
                            }
                            
                        }
                        
                    }
                    
                }
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Handle_Setting_Resgister(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x07); // Bat led * #, 1, 2, 3
    do
    {
        delay_ms(300);
        Function_Handle_Package_Sound_Step1_1();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Password();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Card();
            break;
        case '3':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Fingerprint();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step1_1(void)
{
    do
    {
        Key = AUDIO_Handle_Send(AND_PASSWORD, PRESS_KEY, SPEAK_1, MID_MIN_MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(AND_CARD, PRESS_KEY, SPEAK_2, NORMAL_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(AND_FINGERPRINT, PRESS_KEY, SPEAK_3, MID_MIN_MIN_TIME_SOUND);
                if(Key != 0) break;
                else
                {
                    Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        break;
                    }
                    else
                    {
                        Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                        if(Key != 0) 
                        {
                            Start_Count = 0;
                            break;
                        }
                        else
                        {
                            Start_Count = 0;
                            break;
                        }
                        
                    }
                                
                }
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Handle_Setting_Password(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x03); // Bat led * #, 1, 2
    do
    {
        Function_Handle_Package_Sound_Step1_2();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Resgister();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Password_User();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Password_Manager();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step1_2(void)
{
    do
    {
        Key = AUDIO_Handle_Send(NOTIFICATION_USER, PRESS_KEY, SPEAK_1, MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(NOTIFICATION_MANAGER, PRESS_KEY, SPEAK_2, NORMAL_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                if(Key != 0) 
                {
                    Start_Count = 0;
                    break;
                }
                else
                {
                    Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        break;
                    }
                    else
                    {
                        Start_Count = 0;
                        break;
                    }
                    
                }
                                
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Handle_Setting_Password_User(void)
{
    
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0F, 0xFF); // Bat tat ca led ban phim
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    Check_Key = GET_Password(Password_Array); // Doc key
    Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao
    if(Check_Key == REPEAT)// Kiem tra xem # co duoc nhan hay khong
    {
        LED_Dispaly(0x00, 0x00);
        Check_Password_Enroll = Function_Check_Password_In_Eeprom(Password_Array);
        if(Check_Password_Enroll == FALSE)
        {
            Function_Save_Password_User_Into_Eeprom(Password_Array);
        }
    }
    else if(Check_Key == BACK)  // Kiem tra xem * co duoc nhan hay khong
    {
        Function_Handle_Setting_Password();
    }

}

bool Function_Check_Password_In_Eeprom(uint8_t* Password_Array)
{
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        READ_Frame_From_Eeprom(Password_Save, ADDRESS_KEY_FIRST+16*i, SIZEOFFRAME);
        if(memcmp(Password_Array, Password_Save, Check_Number_Of_Array) == 0)
        {
            AUDIO_Send(PASSWORD_WAS_USED);
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            if(Key == '1')
            {
                Function_Handle_Setting_Password_User();
                return TRUE;
            }
            else if(Key == '2')
            {
                Function_Handle_Setting_Password_Manager();
                return TRUE;
            }
        }
        if(Password_Save[0] == 0xFF)
        {
            Temp_ADDRESS = ADDRESS_KEY_FIRST+16*i;
            Temp_ID = i+1;
            return FALSE;
        }
    }
    return FALSE;
}

void Function_Save_Password_User_Into_Eeprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            Password_Array[i] = Temp_ID; Password_Array[i+1] = USER; Password_Array[i+2] = END_FRAME;
            for(uint8_t j = i+3; j < SIZEOFFRAME; j++)
            {
                Password_Array[j] = 0xFF;
            }
            WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME); // Luu password vao eeprom
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

void Function_Handle_Setting_Password_Manager(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0F, 0xFF); // Bat tat ca led ban phim
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    Check_Key = GET_Password(Password_Array); // Doc key
    Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao
    if(Check_Key == REPEAT)// Kiem tra xem # co duoc nhan hay khong
    {
        LED_Dispaly(0x00, 0x00);
        Check_Password_Enroll = Function_Check_Password_In_Eeprom(Password_Array);
        if(Check_Password_Enroll == FALSE)
        {
            Function_Save_Password_Manager_Into_Eeprom(Password_Array);
        }
    }
    else if(Check_Key == BACK)  // Kiem tra xem * co duoc nhan hay khong
    {
        Function_Handle_Setting_Password();
    }
}

void Function_Save_Password_Manager_Into_Eeprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            Password_Array[i] = Temp_ID; Password_Array[i+1] = MANAGER; Password_Array[i+2] = END_FRAME;
            for(uint8_t j = i+3; j < SIZEOFFRAME; j++)
            {
                Password_Array[j] = 0xFF;
            }
            WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME); // Luu password vao eeprom
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

void Function_Handle_Setting_Card(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x03); // Bat led * #, 1, 2
    do
    {
        Function_Handle_Package_Sound_Step1_2();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Resgister();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Card_User();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Card_Manager();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Setting_Card_User(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat led *
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    Check_Key = GET_Card(Password_Array);
    if(Check_Key == '*')
    {
        AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
        delay_ms(300);
        Function_Handle_Setting_Card();
    }
    else
    {
        LED_Dispaly(0x08, 0x00); // Bat led #
        Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao
        do
        {
            Check_Key = GET_Key();
            if(Check_Key == '#')// Kiem tra xem # co duoc nhan hay khong
            {
                LED_Dispaly(0x00, 0x00);
                AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
                delay_ms(300);
                Check_Card_Enroll = Function_Check_Card_In_Eeprom(Password_Array);
                if(Check_Card_Enroll == FALSE)
                {
                    Function_Save_Card_User_Into_Eeprom(Password_Array);
                    break;
                }
            }

        } while ((Check_Key != '#') && (Sleep_Flag ==0));

    }
    
}


bool Function_Check_Card_In_Eeprom(uint8_t* Password_Array)
{
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        READ_Frame_From_Eeprom(Password_Save, ADDRESS_CARD_FIRST+16*i, SIZEOFFRAME);
        if(memcmp(Password_Array, Password_Save, Check_Number_Of_Array) == 0)
        {
            AUDIO_Send(PASSWORD_WAS_USED);
            memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
            if(Key == '1')
            {
                Function_Handle_Setting_Card_User();
                return TRUE;
            }
            else if(Key == '2')
            {
                Function_Handle_Setting_Card_Manager();
                return TRUE;
            }
        }
        if(Password_Save[0] == 0xFF)
        {
            Temp_ADDRESS = ADDRESS_CARD_FIRST+16*i;
            Temp_ID = i+1;
            return FALSE;
        }
    }
    return FALSE;
}

void Function_Save_Card_User_Into_Eeprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            Password_Array[i] = Temp_ID; Password_Array[i+1] = USER; Password_Array[i+2] = END_FRAME;
            for(uint8_t j = i+3; j < SIZEOFFRAME; j++)
            {
                Password_Array[j] = 0xFF;
            }
            WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
            memset(Password_Array, NULL, SIZEOFFRAME); 
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

void Function_Handle_Setting_Card_Manager(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat led * #
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    Check_Key = GET_Card(Password_Array);
    if(Check_Key == '*')
    {
        AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
        delay_ms(300);
        Function_Handle_Setting_Card();
    }
    else
    {
        LED_Dispaly(0x08, 0x00); // Bat led #
        Check_Number_Of_Array = Function_Check_Number_Of_Array(Password_Array); //Kiem tra so phan tu nhap vao
        do
        {
            Check_Key = GET_Key();
            if(Check_Key == '#')// Kiem tra xem # co duoc nhan hay khong
            {
                LED_Dispaly(0x00, 0x00);
                AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
                delay_ms(300);
                Check_Card_Enroll = Function_Check_Card_In_Eeprom(Password_Array);
                if(Check_Card_Enroll == FALSE)
                {
                    Function_Save_Card_Manager_Into_Eeprom(Password_Array);
                    break;
                }
            }

        } while ((Check_Key != '#') && (Sleep_Flag ==0));

    }
}

void Function_Save_Card_Manager_Into_Eeprom(uint8_t* Password_Array)
{
    for(uint8_t i = 0; i < SIZEOFFRAME; i++)
    {
        if(Password_Array[i] == NULL)
        {
            Password_Array[i] = Temp_ID; Password_Array[i+1] = MANAGER; Password_Array[i+2] = END_FRAME;
            for(uint8_t j = i+3; j < SIZEOFFRAME; j++)
            {
                Password_Array[j] = 0xFF;
            }
            WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME); 
            memset(Password_Array, NULL, SIZEOFFRAME); 
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_MIN_TIME_SOUND);
            Sleep_Flag = 1;
            break;
        }
    }
}

void Function_Handle_Setting_Fingerprint(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x03); // Bat led * #, 1, 2
    do
    {
        Function_Handle_Package_Sound_Step1_2();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Resgister();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Fingerprint_User();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_Fingerprint_Manager();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Setting_Fingerprint_User(void)
{

}

void Function_Handle_Setting_Fingerprint_Manager(void)
{

}

void Function_Handle_Setting_Clear(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x3F); // Bat led * #, 1, 2, 3, 4, 5, 6
    do
    {
        delay_ms(300);
        Function_Handle_Package_Sound_Step2_1();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_One_Key();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_One_Card();
            break;
        case '3':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_One_Fingerprint();
            break;
        case '4':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_All_Key();
            break;
        case '5':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_All_Card();
            break;
        case '6':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Clear_All_Fingerprint();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step2_1(void)
{
    do
    {
        Key = AUDIO_Handle_Send(REMOVE_ONE_PASSWORD, PRESS_KEY, SPEAK_1, NORMAL_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(REMOVE_ONE_CARD, PRESS_KEY, SPEAK_2, NORMAL_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(REMOVE_ONE_FINGERPRINT, PRESS_KEY, SPEAK_3, NORMAL_TIME_SOUND);
                if(Key != 0) break;
                else
                {
                   Key = AUDIO_Handle_Send(REMOVE_ALL_PASSWORD, PRESS_KEY, SPEAK_4, MID_MIN_TIME_SOUND);
                   if(Key != 0) break;
                   else
                   {
                        Key = AUDIO_Handle_Send(REMOVE_ALL_CARD, PRESS_KEY, SPEAK_5, MID_MIN_TIME_SOUND);
                        if(Key != 0) break;
                        else
                        {
                            Key = AUDIO_Handle_Send(REMOVE_ALL_FINGERPRINT, PRESS_KEY, SPEAK_6, MID_MIN_TIME_SOUND);
                            if(Key != 0) break;
                            else
                            {
                                Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                                if(Key != 0) 
                                {
                                    Start_Count = 0;
                                    break;
                                }
                                else
                                {
                                    Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                                    if(Key != 0) 
                                    {
                                        Start_Count = 0;
                                        break;
                                    }
                                    else
                                    {
                                        Start_Count = 0;
                                        break;
                                    }
                                    
                                }
                                            
                            }
                        }    
                   }
                   
                                
                }
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Clear_One_Key(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0F, 0xFF); // Bat all led
    do
    {
        
        Function_Handle_Package_Sound_Step2_2();
        AUDIO_Send(PRESS_KEY);
        ID_Value = Function_Covert_Character_To_ID(ID_Clear);
        if(Key == BACK)
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if((Key == REPEAT) && (ID_Value != 0))
        {
            Function_Handle_Clear_One_Key(ID_Value);
            break;
        }

    }while(Key == REPEAT);
}

void Function_Handle_Package_Sound_Step2_2(void)
{
    do
    {
        Key = AUDIO_Handle_Send(SUGGEST_ENTER_USER_ORDER_NUMBER, 0, 0, MID_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            
            Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
            if(Key != 0) 
            {
                Start_Count = 0;
                break;
            }
            else
            {
                Key = AUDIO_Handle_Send(TO_CONFIRM_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                if(Key != 0) 
                {
                    Start_Count = 0;
                    break;
                }
                else
                {
                    Start_Count = 0;
                    break;
                }
                
            }                
                
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

uint8_t Function_Covert_Character_To_ID(uint8_t* ID_Clear)
{
    int returnvalue = 0, number = 0;
    Key = GET_Password(ID_Clear);
    number = strlen((char const*)ID_Clear);

    for(int i = 0; i < number; i++)
    {
        ID_Clear[i] = (uint8_t)ID_Clear[i]-0x30;
    }
    if(number == 1)
    {
        returnvalue =  ID_Clear[0];
    }
    else if(number == 2)
    {
        returnvalue =  ID_Clear[0]*10 + ID_Clear[1];
    }
    else if(number == 3)
    {
        returnvalue =  ID_Clear[0]*100 + ID_Clear[1]*10 + ID_Clear[2];
    }
    return returnvalue;

}

void Function_Handle_Clear_One_Key(uint8_t ID_Number)
{
    if(ID_Number != 0)
    {
        Temp_ADDRESS = ADDRESS_KEY_FIRST+16*(ID_Number-1);
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
        memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
        AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Handle_Setting_Clear();
    }
    else
    {   AUDIO_Handle_Send(OPERATION_NOT_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Clear_One_Key();
    }
    
}

void Function_Clear_One_Card(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0F, 0xFF); // Bat all led
    do
    {
        Function_Handle_Package_Sound_Step2_2();
        ID_Value = Function_Covert_Character_To_ID(ID_Clear);
        if(Key == BACK)
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if((Key == REPEAT) && (ID_Value != 0))
        {
            Function_Handle_Clear_One_Card(ID_Value);
            break;
        }

    }while(Key == REPEAT);
}

void Function_Handle_Clear_One_Card(uint8_t ID_Number)
{
    if(ID_Number != 0)
    {
        Temp_ADDRESS = ADDRESS_CARD_FIRST+16*(ID_Number-1);
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
        memset(Password_Array, NULL, SIZEOFFRAME); 
        AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Handle_Setting_Clear();
    }
    else
    {
         AUDIO_Handle_Send(OPERATION_NOT_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Clear_One_Card();
    }
}

void Function_Clear_One_Fingerprint(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0F, 0xFF); // Bat all led
    do
    {
        Function_Handle_Package_Sound_Step2_2();
        ID_Value = Function_Covert_Character_To_ID(ID_Clear);
        if(Key == BACK)
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if((Key == REPEAT) && (ID_Value != 0))
        {
            Function_Handle_Clear_One_Fingerprint(ID_Value);
            break;
        }

    }while(Key == REPEAT);
}

void Function_Handle_Clear_One_Fingerprint(uint8_t ID_Number)
{
    if(ID_Number != 0)
    {
        Temp_ADDRESS = ADDRESS_FINGERPRINT_FIRST+16*(ID_Number-1);
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
        memset(Password_Array, NULL, SIZEOFFRAME); 
        AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Handle_Setting_Clear();
    }
    else
    {
        AUDIO_Handle_Send(OPERATION_NOT_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        Function_Clear_One_Fingerprint();
    }
}

void Function_Clear_All_Key(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat LED * #
    do
    {
        Function_Handle_Package_Sound_Step2_3();
        if(Key == '*')
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if(Key == '#')
        {
            Function_Handle_Clear_All_Key();
            break;
        }

    }while((Key != '#') && (Sleep_Flag == 0));
}

void Function_Handle_Package_Sound_Step2_3(void)
{
    do
    {
        Key = AUDIO_Handle_Send(TO_CONFIRM_PRESS_THANG, 0, 0, MID_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            
            Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
            if(Key != 0) 
            {
                Start_Count = 0;
                break;
            }
            else
            {
                Start_Count = 0;
                break;
            }                
                
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Handle_Clear_All_Key(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x00, 0x00); // Off led
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        Temp_ADDRESS = ADDRESS_KEY_FIRST+16*i;
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
    }
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
    Function_Handle_Setting_Clear();
}

void Function_Clear_All_Card(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat LED * #
    do
    {
        Function_Handle_Package_Sound_Step2_3();
        Key = GET_Key();
        if(Key == '*')
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if(Key == '#')
        {
            Function_Handle_Clear_All_Card();
            break;
        }  

    }while((Key != '#') && (Sleep_Flag == 0));
}

void Function_Handle_Clear_All_Card(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x00, 0x00); // off led
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        Temp_ADDRESS = ADDRESS_CARD_FIRST+16*i;
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
    }
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
    Function_Handle_Setting_Clear();
}

void Function_Clear_All_Fingerprint(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat LED * #
    do
    {
        Function_Handle_Package_Sound_Step2_3();
        Key = GET_Key();
        if(Key == '*')
        {
            Function_Handle_Setting_Clear();
            break;
        }
        else if(Key == '#')
        {
            Function_Handle_Clear_All_Fingerprint();
            break;
        }

    }while((Key != '#') && (Sleep_Flag == 0));
}

void Function_Handle_Clear_All_Fingerprint(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x00, 0x00); // off led
    for(uint16_t i = 0; i < ALL_FRAME_USE; i++)
    {
        Temp_ADDRESS = ADDRESS_FINGERPRINT_FIRST+16*i;
        memset(Password_Array, 0xFF, SIZEOFFRAME);
        WRITER_Frame_To_Eeprom(Temp_ADDRESS, Password_Array, SIZEOFFRAME);
    }
    memset(Password_Array, NULL, SIZEOFFRAME); // Xoa tat ca key da nhap
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
    Function_Handle_Setting_Clear();
}

void Function_Handle_Setting_System(void)
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x0F); // Bat led * #, 1, 2, 3, 4
    do
    {
        Function_Handle_Package_Sound_Step3_1();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Volume();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Language();
            break;
        case '3':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Status_Left_Lock();
            break;
        case '4':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Recover_Setting_Original();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step3_1(void)
{
    do
    {
        Key = AUDIO_Handle_Send(ADJ_SOUND, PRESS_KEY, SPEAK_1, NORMAL_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(SETTING_LANGUAGE, PRESS_KEY, SPEAK_2, NORMAL_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(SETTING_LOCK_LEFT, PRESS_KEY, SPEAK_3, MID_MIN_TIME_SOUND);
                if(Key != 0) break;
                else
                {
                    Key = AUDIO_Handle_Send(SYSTEM_RECOVERY, PRESS_KEY, SPEAK_4, MID_MIN_TIME_SOUND);
                    if(Key != 0) break;
                    else
                    {
                        Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                        if(Key != 0) 
                        {
                            Start_Count = 0;
                            break;
                        }
                        else
                        {
                            Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                            if(Key != 0) 
                            {
                                Start_Count = 0;
                                break;
                            }
                            else
                            {
                                Start_Count = 0;
                                break;
                            }
                            
                        }   
                    }
                                
                }
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Setting_Volume()
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x0F); // Bat led * #, 1, 2, 3, 4
    do
    {
        Function_Handle_Package_Sound_Step3_1_1();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_System();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Loud_Volume();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Mid_Volume();
            break;
        case '3':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Small_Volume();
            break;
        case '4':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Clam_Volume();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step3_1_1(void)
{
    do
    {
        Key = AUDIO_Handle_Send(MAX_SOUND, PRESS_KEY, SPEAK_1, MID_MIN_MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(MIND_SOUND, PRESS_KEY, SPEAK_2, MID_MIN_MIN_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(MIN_SOUND, PRESS_KEY, SPEAK_3, MID_MIN_MIN_TIME_SOUND);
                if(Key != 0) break;
                else
                {
                    Key = AUDIO_Handle_Send(CLAM_SOUND, PRESS_KEY, SPEAK_4, MID_MIN_MIN_TIME_SOUND);
                    if(Key != 0) break;
                    else
                    {
                        Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                        if(Key != 0) 
                        {
                            Start_Count = 0;
                            break;
                        }
                        else
                        {
                            Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                            if(Key != 0) 
                            {
                                Start_Count = 0;
                                break;
                            }
                            else
                            {
                                Start_Count = 0;
                                break;
                            }
                            
                        }   
                    }
                                
                }
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Setting_Loud_Volume()
{
    AUDIO_Send(BIG_SOUND);
    delay_ms(150);
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_Mid_Volume()
{
    AUDIO_Send(NORMAL_SOUND);
    delay_ms(150);
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_Small_Volume()
{
    AUDIO_Send(NORMAL_SOUND);
    delay_ms(150);
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_Clam_Volume()
{
    AUDIO_Send(NOT_SOUND);
    delay_ms(150);
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_Language()
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x03); // Bat led * #, 1, 2
    do
    {
        Function_Handle_Package_Sound_Step3_2();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_System();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Vietnamese_Language();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_English_Language();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step3_2(void)
{
    do
    {
        Key = AUDIO_Handle_Send(VIETNAMESE, PRESS_KEY, SPEAK_1, MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(ENGLISH, PRESS_KEY, SPEAK_2, MIN_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                if(Key != 0) 
                {
                    Start_Count = 0;
                    break;
                }
                else
                {
                    Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        break;
                    }
                    else
                    {
                        Start_Count = 0;
                        break;
                    }
                    
                }
                                
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Setting_Vietnamese_Language(void)
{
    Check_Language = 0;
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_MIN_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_English_Language(void)
{
    Check_Language = 1;
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_MIN_TIME_SOUND);
    Function_Handle_Setting_System();
}

void Function_Setting_Status_Left_Lock()
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x03); // Bat led * #, 1, 2
    do
    {
        Function_Handle_Package_Sound_Step3_3();
        Key = GET_Key();
        switch (Key)
        {
        case '*':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_System();
            break;
        case '#':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            break;
        case '1':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Left_Lock();
            break;
        case '2':
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Setting_Normal();
            break;
        default:
            delay_ms(300);
            break;
        }
    }while(Key == '#');
}

void Function_Handle_Package_Sound_Step3_3(void)
{
    do
    {
        Key = AUDIO_Handle_Send(USE_LOCK_LEFT, PRESS_KEY, SPEAK_1, MID_MIN_TIME_SOUND);
        if(Key != 0) break;
        else
        {
            Key = AUDIO_Handle_Send(UNUSER_LOCK_LEFT, PRESS_KEY, SPEAK_2, MID_MIN_TIME_SOUND);
            if(Key != 0) break;
            else 
            {
                Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, MID_TIME_SOUND);
                if(Key != 0) 
                {
                    Start_Count = 0;
                    break;
                }
                else
                {
                    Key = AUDIO_Handle_Send(LISTEN_AGAIN_PRESS_THANG, 0, 0, MID_TIME_SOUND);
                    if(Key != 0) 
                    {
                        Start_Count = 0;
                        break;
                    }
                    else
                    {
                        Start_Count = 0;
                        break;
                    }
                    
                }
                                
                
            }
        }
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Setting_Left_Lock(void)
{
    // Viet function enable leftlock
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
    Use_Left_Lock_State = 1;
    Sleep_Flag = 1;
}

void Function_Setting_Normal(void)
{
    // Viet function disable leftlock
    AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
    Use_Left_Lock_State = 0;
    Sleep_Flag = 1;
}

void Function_Recover_Setting_Original()
{
    LED_Control_Power_On(); // Cap nguon cho led
    LED_Dispaly(0x0A, 0x00); // Bat led * #
    do
    {
        Function_Handle_Package_Sound_Step4();
        Key = GET_Key();
        if(Key == '*')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            Function_Handle_Setting_System();
            break;
        }
        else if(Key == '#')
        {
            AUDIO_Send(SOUND_WHEN_PRESS_NUMBER);
            delay_ms(300);
            memset(Password_Array, 0xFF, SIZEOFFRAME);
            for(uint16_t i = 0; i < ALL_FRAME_USE_COMMON; i++)
            {
                WRITER_Frame_To_Eeprom(ADDRESS_KEY_FIRST+16*i, Password_Array, SIZEOFFRAME);
            }
            AUDIO_Send(BIG_SOUND); // Default sound
            Check_Language = 0; // Vietnamese
            AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, MID_MIN_TIME_SOUND);
            Sleep_Flag = 1;
        }

    } while ((Key != '#') && (Sleep_Flag == 0));

}

void Function_Handle_Package_Sound_Step4(void)
{
    do
    {
        Key = AUDIO_Handle_Send(TO_CONFIRM_PRESS_THANG, 0, 0, MID_MIN_TIME_SOUND);
        if(Key != 0) 
        {
            Start_Count = 0;
            break;
        }
        else
        {
            Key = AUDIO_Handle_Send(TO_BACK_PRESS_SAO, 0, 0, NORMAL_TIME_SOUND);
            if(Key != 0) 
            {
                Start_Count = 0;
                break;
            }
            else
            {
                Start_Count = 0;
                break;
            }
            
        }                
                
        
    } while ((Key == 0) && (Start_Count == 1));
}

void Function_Handle_Setting_Introduce(void)
{
    uint8_t code = 0;
    AUDIO_Handle_Send(NUMBER_BOARD, 0, 0, NORMAL_TIME_SOUND);
    for(uint8_t i = 4; i < 9; i ++)
    {
        code = READ_Byte_From_Eeprom(ADDRESS_DEVICE_CODE_FIRST+i);
        switch(code)
        {
        case '0':
            AUDIO_Handle_Send(SPEAK_0, 0, 0, MIN_TIME_SOUND);
            break;
        case '1':
            AUDIO_Handle_Send(SPEAK_1, 0, 0, MIN_TIME_SOUND);
            break;
        case '2':
            AUDIO_Handle_Send(SPEAK_2, 0, 0, MIN_TIME_SOUND);
            break;
        case '3':
            AUDIO_Handle_Send(SPEAK_3, 0, 0, MIN_TIME_SOUND);
            break;
        case '4':
            AUDIO_Handle_Send(SPEAK_4, 0, 0, MIN_TIME_SOUND);
            break;
        case '5':
            AUDIO_Handle_Send(SPEAK_5, 0, 0, MIN_TIME_SOUND);
            break;
        case '6':
            AUDIO_Handle_Send(SPEAK_6, 0, 0, MIN_TIME_SOUND);
            break;
        case '7':
            AUDIO_Handle_Send(SPEAK_7, 0, 0, MIN_TIME_SOUND);
            break;
        case '8':
            AUDIO_Handle_Send(SPEAK_8, 0, 0, MIN_TIME_SOUND);
            break;
        case '9':
            AUDIO_Handle_Send(SPEAK_9, 0, 0, MIN_TIME_SOUND);
            break;
        default:
            break;
        }
    }
    Sleep_Flag = 1;

}

void Function_Check_Fingerprint(void)
{

}

void Function_Handle_Join_Network(void)
{
    Function_Join_Network();
    for(uint8_t i = 0; i < 5; i++)
    {
        NCP_Proc();
        delay_ms(100);   
        if(zb_flagjoinnetwork == 1)
        {
            break;
        }
    }

    if(zb_flagjoinnetwork == 1)
    {
        AUDIO_Handle_Send(OPERATION_SUCCESSFUL, 0, 0, NORMAL_TIME_SOUND);
        zb_flagjoinnetwork = 0;
        Sleep_Flag = 1;
    }
}

void Function_Network_Info()
{
    ZB_Wakeup();
    NCP_Send(Buffer_vtupdatenetworkinfo, sizeof(Buffer_vtupdatenetworkinfo));
    if(zb_flagnetworkinfo == 1)
    {
        if(networkstatus == 0x00)
        {
            // SPEAK_Notification("Thiet bi da gia nhap mang");
            LED_Control_Power_On(); // Cap nguon cho led
            LED_Dispaly(0x00, 0x01);
        }
        else if(networkstatus == 0xFF)
        {
            // SPEAK_Notification("Thiet bi chua gia nhap mang");
            LED_Control_Power_On(); // Cap nguon cho led
            LED_Dispaly(0x00, 0x02); 
        }
        zb_flagnetworkinfo = 0;
    }
}

void Function_Join_Network(void)
{
    LED_Control_Power_On();
    ZB_Wakeup();
    NCP_Send(Buffer_vtupdatenetwork, sizeof(Buffer_vtupdatenetwork));
    while(Sleep_Flag == 0)
    {
        LED_Dispaly(0x00, 0x01);
        delay_ms(100);
        LED_Dispaly(0x00, 0x02);
        delay_ms(100);
        LED_Dispaly(0x00, 0x04);
        delay_ms(100);
        LED_Dispaly(0x00, 0x20);
        delay_ms(100);
        LED_Dispaly(0x01, 0x00);
        delay_ms(100);
        LED_Dispaly(0x08, 0x00);
        delay_ms(100);
        LED_Dispaly(0x04, 0x00);
        delay_ms(100);
        LED_Dispaly(0x02, 0x00);
        delay_ms(100);
        LED_Dispaly(0x00, 0x40);
        delay_ms(100);
        LED_Dispaly(0x00, 0x08);
        delay_ms(100);
    }
    
}

void Function_Check_Sleep(void)
{

    if(Sleep_Flag == 1)
    {   
        //Vao sleep mode
        if(Bell_Flag == 1)
        {
            Bell_Flag = 0;
            SLEEP_Mode();
        }
        else if(Switch_Flag == 1)
        {
            Switch_Flag = 0;
            SLEEP_Mode();
        }
        else
        {
            LED_Dispaly(0x00, 0x00);
            AUDIO_Handle_Send(SYSTEM_HAS_EXITED, 0, 0, MID_MIN_MIN_TIME_SOUND);   
            SLEEP_Mode();
        }

    }
}

