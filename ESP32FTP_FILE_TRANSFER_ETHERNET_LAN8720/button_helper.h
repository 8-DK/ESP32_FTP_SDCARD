/** @file button_helper.h
    @brief Function prototypes for the SD card helpers.

    This contains the prototypes for the SD card
    functionalities and eventually any macros, constants,
    or global variables you will need.

    @author Dhananjay Khairnar
*/
#ifndef __BUTTON_HELPER_H
#define __BUTTON_HELPER_H
#include <Arduino.h>

/**Button 1 gpio definat.io*/
#define BOTTON_1 34
/**Button 2 gpio definat.io*/
#define BOTTON_2 35
/**Buttons debaunce delay*/
#define DEBAUNCE_DELAY 50

void initButtons();
bool isButtonPressed(uint8_t button);
#endif
