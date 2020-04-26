/** @file SD_card_helper.h
 *  @brief Function prototypes for the SD card helpers.
 *
 *  This contains the prototypes for the SD card
 *  functionalities and eventually any macros, constants,
 *  or global variables you will need.
 *
 *  @author Dhananjay Khairnar    
*/
#ifndef __SD_CARD_HELPER_H
#define __SD_CARD_HELPER_H
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "EEPROM.h"

/**Maximum size of EEPROM we are using*/
#define EEPROM_SIZE 128
/**Comment this macro to disable sdcard file logs*/
#define SDCARD_LOG

#ifdef SDCARD_LOG
#define SDCARDLog(...) Serial.print(__VA_ARGS__)
#define SDCARDLogLn(...) Serial.println(__VA_ARGS__)
#define SDCARDLogF(...) Serial.printf(__VA_ARGS__)
#else
#define SDCARDLog(...)
#define SDCARDLogLn(...)
#define SDCARDLogF(...)
#endif


/*
 * Variable expose form SD_card_helper.cpp
 */
/** This variable maintains file read location can be store in eeprom using \a writeEEPROMData() function*/
extern uint32_t readFilePointer;
/** This variable maintains file write location can be store in eeprom using \a writeEEPROMData() function*/
extern uint32_t writeFilePointer;
/** This variable is used to store in eeprom */
extern unsigned int epVariable;
/** This variable is true if file reading is completed, this variable will be false when file reading is started again. \a SD_ReadFile() used in this function*/
extern uint8_t fileReadFlag;

bool SD_Initialize();
bool SD_Detect();
bool SD_Exist(const char * path);
bool SD_mkDir(const char * path);
bool SD_mkFile(const char * path);
bool SD_rmDir(const char * path);
bool SD_rmFile(const char * path);
bool SD_CopyFile(const char * src , const char * dest);
uint32_t SD_ReadFile(uint64_t address, uint8_t *buf , uint32_t len, const char * path);
uint32_t SD_WriteFile(uint8_t * buf , uint32_t len, const char * path);
uint64_t Get_File_Size( const char * path);
void SD_ListDir(const char * dirname, uint8_t levels);
bool SD_RenameFile(const char * path1, const char * path2);

void writeEEPROMData();
void readEEPROMData();




#endif
