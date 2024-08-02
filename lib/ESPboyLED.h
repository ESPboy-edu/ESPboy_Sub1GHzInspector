/*
ESPboy LED class
for www.ESPboy.com project by RomanS
*/
#pragma once

#include <Arduino.h>
#include "ESPboyMCP.h" //to control LED lock

#ifndef ESPboy_LED
#define ESPboy_LED

#define LEDPIN D4
#define LEDLOCK 9

class ESPboyLED{
private:
  ESPboyMCP *mcp; 
  uint8_t LEDr, LEDg, LEDb, LEDflagOnOff;
  void ledset(uint8_t rled, uint8_t gled, uint8_t bled);
  
public: 
  void begin(ESPboyMCP *mcpGUI);
  void off();
  void on();
  uint8_t getState();
  
  void setRGB (uint8_t red, uint8_t green, uint8_t blue);
  void setR (uint8_t red);
  void setG (uint8_t green);
  void setB (uint8_t blue);
  
  uint32_t getRGB();
  uint8_t getR();
  uint8_t getG();
  uint8_t getB();
};  

#endif
