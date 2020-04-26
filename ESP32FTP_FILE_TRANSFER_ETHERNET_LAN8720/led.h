/** @file led.h
    @brief Function prototypes for led.

    This contains the prototypes for the Led
    and eventually any macros, constants,
    or global variables you will need.

    @author Dhananjay Khairnar
*/
#ifndef __LED_H
#define __LED_H
#include <Arduino.h>

#define LED_1_PIN 2
#define LED_2_PIN 32
#define LED_3_PIN 33
#define LED_4_PIN 5

/** Enum for LED LED numbers  */
typedef enum LED_NUM
{
  LED_1 = 1,
  LED_2,
  LED_3,
  LED_4,
}LED_NUM;

void initLED();

void setLed(uint8_t led, uint8_t val);


#endif
