/** @file button_helper.cpp
 *  @brief Function implimentation for the SD card helpers.
 *
 *  This contains the implimentation for the SD card
 *  functionalities.
 *
 *  @author Dhananjay Khairnar    
*/
#include "button_helper.h"

uint8_t button1pressed = 0;
uint8_t button2pressed = 0;

/** @brief ISR for button 1 press event
       
    @return void
*/
void IRAM_ATTR isr1() 
{
  button1pressed += 1;
}

/** @brief ISR for button 2 press event
       
    @return void
*/
void IRAM_ATTR isr2() {
  button2pressed += 1;
}

/** @brief Initialize button GPIO
       
    @return void
*/
void initButtons()
{
  pinMode(BOTTON_1, INPUT_PULLUP);
  attachInterrupt(BOTTON_1, isr1, FALLING);

  pinMode(BOTTON_2, INPUT_PULLUP);
  attachInterrupt(BOTTON_2, isr2, FALLING);
}

/** @brief Get the button pressed state

    this function read flag \a button1pressed and \a button2pressed which is set
    by ISR. in this function this flag are get cleared.
    
    @param button BOTTON_1/BOTTON_2
    
    @return true if button pressed/ false if no button pressed
*/
bool isButtonPressed(uint8_t button)
{
  if (BOTTON_1 == button)
  {
    if (button1pressed > 0)
    {
      delay(DEBAUNCE_DELAY);
      button1pressed = 0;
      return true;
    }
    else
    {
      return false;
    }
  }

  if (BOTTON_2 == button)
  {
    if (button2pressed > 0)
    {
      delay(DEBAUNCE_DELAY);
      button2pressed = 0;
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}

