#ifndef __SD_CARD_HELPER_H
#define __SD_CARD_HELPER_H
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define DATA_BUF_LEN 500

bool SD_Initialize();
bool SD_Detect();
bool SD_Exist(const char * path);
bool SD_mkDir(const char * path);
bool SD_mkFile(const char * path);
bool SD_rmDir(const char * path);
bool SD_rmFile(const char * path);
bool SD_CopyFile(const char * src , const char * dest);
uint8_t SD_ReadFile(uint64_t address, uint8_t *buf , uint8_t len, const char * path);
uint8_t SD_WriteFile(uint8_t * buf , uint8_t len, const char * path);


void listDir(const char * dirname, uint8_t levels);
void listDir(const char * dirname, uint8_t levels);
void removeDir(const char * path);
void readFile(const char * path);
void readFile(const char * path);
void writeFile(const char * path, const char * message);
void appendFile(const char * path, const char * message);
void renameFile(const char * path1, const char * path2);
void testFileIO(const char * path);

#endif
