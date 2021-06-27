//*******************************************************************************************
//
// file:      AP_DCC_Button.cpp
// author:    Jack Christensen / Modified by Aiko Pras
// history:   2021-06-27 V1.1   The code is originally written by Jack Christensen
//                              https://github.com/JChristensen/JC_Button
//                              Small modifications are made by Aiko Pras to make the class
//                              interface resemble the other AP_DCC and RSBus libraries
//
// purpose:   Reads the status of (debounced) buttons
//
// Copyright: Jack Christensen (2018) and licensed under
//            GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************************
#include "AP_DCC_Button.h"


//*******************************************************************************************
// attach and initialize a DCC_Button object and the pin it's connected to.
//*******************************************************************************************
void DCC_Button::attach(uint8_t pin, unsigned long dbTime, bool puEnable, bool invert) {
  m_pin = pin;
  m_dbTime = dbTime;
  m_puEnable = puEnable;
  m_invert = invert;
  // Note that the question mark `?` in `m_puEnable ? INPUT_PULLUP : INPUT`
  // is commonly referred to as the conditional operator. Its meaning is:
  // if m_puEnable is true, `INPUT_PULLUP` is used, otherwise `INPUT`.
  pinMode(m_pin, m_puEnable ? INPUT_PULLUP : INPUT);
  m_state = digitalRead(m_pin);
  if (m_invert) m_state = !m_state;
  m_time = millis();
  m_lastState = m_state;
  m_changed = false;
  m_lastChange = m_time;
}

//*******************************************************************************************
// returns the state of the button, true if pressed, false if released.
// does debouncing, captures and maintains times, previous state, etc.
//*******************************************************************************************
bool DCC_Button::read() {
  unsigned long ms = millis();
  bool pinVal = digitalRead(m_pin);
  if (m_invert) pinVal = !pinVal;
  if (ms - m_lastChange < m_dbTime)  {
    m_changed = false;
  }
  else {
    m_lastState = m_state;
    m_state = pinVal;
    m_changed = (m_state != m_lastState);
    if (m_changed) m_lastChange = ms;
  }
  m_time = ms;
  return m_state;
}

//*******************************************************************************************
// isPressed() and isReleased() check the button state when it was last read, and return
// false (0) or true (!=0) accordingly.
// These functions do not cause the button to be read.
//*******************************************************************************************
bool DCC_Button::isPressed() {
  return m_state;
}

bool DCC_Button::isReleased() {
  return !m_state;
}

//*******************************************************************************************
// wasPressed() and wasReleased() check the button state to see if it changed between the
// last two reads and return false (0) or true (!=0) accordingly.
// These functions do not cause the button to be read.
//*******************************************************************************************
bool DCC_Button::wasPressed() {
  return m_state && m_changed;
}

bool DCC_Button::wasReleased() {
  return !m_state && m_changed;
}

//*******************************************************************************************
// pressedFor(ms) and releasedFor(ms) check to see if the button is pressed (or released),
// and has been in that state for the specified time in milliseconds.
// Returns false (0) or true (!=0) accordingly.
// These functions do not cause the button to be read.
//*******************************************************************************************
bool DCC_Button::pressedFor(unsigned long ms) {
  return m_state && m_time - m_lastChange >= ms;
}

bool DCC_Button::releasedFor(unsigned long ms) {
  return !m_state && m_time - m_lastChange >= ms;
}

//*******************************************************************************************
// lastChange() returns the time the button last changed state, in milliseconds.                                                     *
//*******************************************************************************************
unsigned long DCC_Button::lastChange() {
  return m_lastChange;
}
