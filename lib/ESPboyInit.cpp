/*
ESPboy_Init class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v1.0
*/

#ifndef ESPboy_Init_cpp
#define ESPboy_Init_cpp

#include "ESPboyInit.h"

ESPboyInit::ESPboyInit(){};

void ESPboyInit::begin(const char *appName) {
  //Serial.begin(115200); //serial init
  WiFi.mode(WIFI_OFF); // to safe battery power


//mcp23017 init for buttons, LED LOCK and TFT Chip Select pins
  mcp.begin();
  delay(100);
  for (int i=0;i<8;i++){  
     mcp.pinMode(i, INPUT);
     mcp.pullUp(i, HIGH);}

//DAC init and backlit off
  mcp.writeDAC(0, false);

//LED init
  myLED.begin(&this->mcp);
  myLED.setRGB(0,0,0);

//sound init and test
  pinMode(SOUNDPIN, OUTPUT);
  //playTone(200, 100); 
  //delay(100);
  //playTone(100, 100);
  //delay(100);
  //noPlayTone();
  
//LCD TFT init
  mcp.pinMode(CSTFTPIN, OUTPUT);
  mcp.digitalWrite(CSTFTPIN, LOW);
  tft.begin();
  tft.setSwapBytes(true);
  delay(100);
  //tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

//draw ESPboylogo  
  tft.drawXBitmap(30, 24, ESPboyLogo, 68, 64, TFT_YELLOW);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString (appName, (128-(strlen(appName)*6))/2, 102);

//LCD backlit fading on
  for (uint16_t bcklt=300; bcklt<2500; bcklt+=30){
    mcp.writeDAC(bcklt, false);
    delay(10);}

  delay(1000);

//clear TFT and backlit on high
  mcp.writeDAC(4095, true);
  tft.fillScreen(TFT_BLACK);
};


void ESPboyInit::playTone(uint16_t frq, uint16_t dur) { tone(SOUNDPIN, frq, dur); }
void ESPboyInit::playTone(uint16_t frq) { tone(SOUNDPIN, frq); }

void ESPboyInit::noPlayTone() { noTone(SOUNDPIN); }

uint8_t ESPboyInit::getKeys() { return (~mcp.readGPIOAB() & 255); }

#endif