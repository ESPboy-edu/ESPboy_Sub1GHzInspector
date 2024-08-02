/*
ESPboy_Init class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v1.0
*/

#ifndef ESPboy_Init
#define ESPboy_Init

#include <Arduino.h>
#include "ESPboyMCP.h"
#include "ESPboyMCP.cpp"
#include "ESPboyLED.h"
#include "ESPboyLED.cpp"
#include "ESPboylogo.h"

#include <TFT_eSPI.h> 

#include <ESP8266WiFi.h>       //to control WiFi

#include <FS.h> 
using fs::FS;

#define MCP4725address  0x60
#define CSTFTPIN 		8  //Chip Select pin for LCD (it's on the MCP23017 GPIO expander GPIO8)

#define LEDPIN         D4
#define SOUNDPIN       D3

#define PAD_LEFT        0x01
#define PAD_UP          0x02
#define PAD_DOWN        0x04
#define PAD_RIGHT       0x08
#define PAD_ACT         0x10
#define PAD_ESC         0x20
#define PAD_LFT         0x40
#define PAD_RGT         0x80
#define PAD_ANY         0xff


class ESPboyInit{

public:
  ESPboyMCP mcp;
  TFT_eSPI tft;
  ESPboyLED myLED;


  ESPboyInit();
  void begin(const char *appName);
  
  uint8_t getKeys();
  void playTone(uint16_t frq, uint16_t dur);
  void playTone(uint16_t frq);
  void noPlayTone();
};

#endif
