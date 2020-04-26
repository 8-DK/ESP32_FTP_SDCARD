/** @file led.cpp
    @brief Function implimentation for led.

    This contains the implimentation for the led
    functionalities.

    @author Dhananjay Khairnar
*/
#include "led.h"

/** @brief This function initialize led gpio
      
    @return void
*/
void initLED()
{
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
}

/** @brief This function set value to the led 
    
    @param led led number LED_1,LED_2,LED_3,LED_4,LED_5,LED_6,LED_7,LED_8
    @param val value to be set 0 or 1
    
    @return updated crc
*/
void setLed(uint8_t led, uint8_t val)
{
  if (led == LED_1)
  {
    digitalWrite (LED_1_PIN, val & 0x01);
  }
  else if (led == LED_2)
  {
    digitalWrite (LED_2_PIN, val & 0x01);
  }
  else if (led == LED_3)
  {
    digitalWrite (LED_3_PIN, val & 0x01);
  }
  else if (led == LED_4)
  {
    digitalWrite (LED_4_PIN, val & 0x01);
  }
}

