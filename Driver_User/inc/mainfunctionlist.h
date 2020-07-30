/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAINFUNCTIONLIST_H
#define __MAINFUNCTIONLIST_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stdbool.h"

bool Function_Check_Card(void);
void Function_Check_Button(void);
void Function_Check_Fingerprint(void);
void Function_Check_All_Flag(void);
void Function_Check_Sleep(void);
uint8_t Function_Check_Number_Of_Array(uint8_t* array);
bool Function_Check_AllByte_Use_In_Eeprom(void);
void Function_Save_Password_into_Eepprom(uint8_t* Password_Array);
void Function_Save_Password_User_Into_Eeprom(uint8_t* Password_Array);
void Function_Save_Password_Manager_Into_Eeprom(uint8_t* Password_Array);
bool Function_Check_Password(uint8_t* Password_Array);
bool Function_Compare_Password(uint8_t *Password_Save, uint8_t *Password_Enter);
bool Function_Check_Password_In_Eeprom(uint8_t* Password_Array);
void Function_Handle_Password_Correct(void);
void Function_Handle_Password_Not_Correct(void);
void Function_Handle_Card_Correct(void);
void Function_Handle_Card_Not_Correct(void);
bool Function_Check_Password_Manager_Or_User(uint8_t Check_Number_Of_Array);
bool Function_Check_Card_Manager_Or_User(uint8_t Check_Number_Of_Array);
void Function_Handle_Password_Manager(void);
void Function_Handle_Password_User(void);
void Function_Handle_Card_User(void);
void Function_Handle_Card_Manager(void);
uint8_t Function_Handle_Setting(void);
void Function_Handle_Setting_Resgister(void);
void Function_Handle_Setting_Clear(void);
void Function_Handle_Setting_System(void);
void Function_Handle_Setting_Introduce(void);
void Function_Handle_Setting_Password(void);
void Function_Handle_Setting_Card(void);
void Function_Handle_Setting_Fingerprint(void);
void Function_Handle_Setting_Password_User(void);
void Function_Handle_Setting_Password_Manager(void);
void Function_Handle_Setting_Card_User(void);
void Function_Handle_Setting_Card_Manager(void);
void Function_Handle_Setting_Fingerprint_User(void);
void Function_Handle_Setting_Fingerprint_Manager(void);
void Function_Clear_One_Key(void);
void Function_Clear_One_Card(void);
void Function_Clear_One_Fingerprint(void);
void Function_Clear_All_Key(void);
void Function_Clear_All_Card(void);
void Function_Clear_All_Fingerprint(void);
uint8_t Function_Covert_Character_To_ID(uint8_t* ID_Clear);
void Function_Handle_Clear_One_Key(uint8_t ID_Number);
void Function_Handle_Clear_One_Card(uint8_t ID_Number);
void Function_Handle_Clear_One_Fingerprint(uint8_t ID_Number);
void Function_Handle_Clear_All_Fingerprint(void);
void Function_Handle_Clear_All_Key(void);
void Function_Handle_Clear_All_Card(void);
void Function_Setting_Volume(void);
void Function_Setting_Language(void);
void Function_Setting_Status_Left_Lock(void);
void Function_Recover_Setting_Original(void);
void Function_Setting_Loud_Volume(void);
void Function_Setting_Mid_Volume(void);
void Function_Setting_Small_Volume(void);
void Function_Setting_Clam_Volume(void);
void Function_Setting_Vietnamese_Language(void);
void Function_Setting_English_Language(void);
void Function_Setting_Left_Lock(void);
void Function_Setting_Normal(void);
bool Function_Check_Card_In_Eeprom(uint8_t* Password_Array);
void Function_Save_Card_User_Into_Eeprom(uint8_t* Password_Array);
bool Function_Handle_Card(uint8_t* Password_Array);
void Function_Save_Card_Into_Eeprom(uint8_t* Password_Array);
void Function_Save_Card_Manager_Into_Eeprom(uint8_t* Password_Array);
void Function_Handle_Join_Network(void);
void Function_Network_Info(void);
void Function_Join_Network(void);
void Function_Notification_Id(void);
void Function_Handle_Package_Sound_Step1(void);
void Function_Handle_Package_Sound_Step1_1(void);
void Function_Handle_Package_Sound_Step1_2(void);
void Function_Handle_Package_Sound_Step2_1(void);
void Function_Handle_Package_Sound_Step2_2(void);
void Function_Handle_Package_Sound_Step2_3(void);
void Function_Handle_Package_Sound_Step3_1(void);
void Function_Handle_Package_Sound_Step3_1_1(void);
void Function_Handle_Package_Sound_Step3_2(void);
void Function_Handle_Package_Sound_Step3_3(void);
void Function_Handle_Package_Sound_Step4(void);

#endif //__MAINFUNCTIONLIST_H