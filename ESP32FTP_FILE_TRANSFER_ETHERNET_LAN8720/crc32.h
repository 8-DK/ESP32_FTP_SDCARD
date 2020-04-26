/** @file crc32.h
    @brief Function prototypes for CRC32 calculation.

    This contains the prototypes for the CRC32 calculation.
    and eventually any macros, constants,
    or global variables you will need.

    @author Dhananjay Khairnar
*/
#ifndef __CRC32_H
#define __CRC32_H
#include <Arduino.h>

/**Comment this macro to disable crc logs */
#define CRC32_Log

#ifdef CRC32_Log
#define CRC32Log(...) Serial.print(__VA_ARGS__)
#define CRC32LogLn(...) Serial.println(__VA_ARGS__)
#define CRC32LogF(...) Serial.printf(__VA_ARGS__)
#else
#define CRC32Log(...)
#define CRC32LogLn(...)
#define CRC32LogF(...)
#endif

uint32_t getFileCrc(const char * path);

#endif
