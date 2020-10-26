/*
ESPboy_Init class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v1.0
*/

#include "ESPboyInit.h"

ESPboyInit::ESPboyInit(){};

void ESPboyInit::begin(char *appName) {
  //Serial.begin(115200); //serial init
  WiFi.mode(WIFI_OFF); // to safe battery power

//DAC init and backlit off
  dac.begin(MCP4725address);
  delay (100);
  dac.setVoltage(0, false);

//mcp23017 init for buttons, LED LOCK and TFT Chip Select pins
  mcp.begin(MCP23017address);
  delay(100);
  
  for (int i=0;i<8;i++){  
     mcp.pinMode(i, INPUT);
     mcp.pullUp(i, HIGH);}

//sound init and test
  pinMode(SOUNDPIN, OUTPUT);
  playTone(200, 100); 
  delay(100);
  playTone(100, 100);
  delay(100);
  noPlayTone();
  
//LCD TFT init
  mcp.pinMode(CSTFTPIN, OUTPUT);
  mcp.digitalWrite(CSTFTPIN, LOW);
  tft.begin();
  delay(100);
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

//draw ESPboylogo  
  tft.drawXBitmap(30, 24, ESPboyLogo, 68, 64, TFT_YELLOW);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString (appName, (128-(strlen(appName)*6))/2, 102);

//LCD backlit fading on
  for (uint16_t bcklt=300; bcklt<4095; bcklt+=30){
    dac.setVoltage(bcklt, false);
    delay(10);}

//clear TFT and backlit on high
  dac.setVoltage(4095, true);
  tft.fillScreen(TFT_BLACK);

//LED pin LOCK OFF
  mcp.pinMode(LEDLOCK, OUTPUT);
  mcp.digitalWrite(LEDLOCK, HIGH); 
};


void ESPboyInit::playTone(uint16_t frq, uint16_t dur) { tone(SOUNDPIN, frq, dur); }
void ESPboyInit::playTone(uint16_t frq) { tone(SOUNDPIN, frq); }

void ESPboyInit::noPlayTone() { noTone(SOUNDPIN); }

uint8_t ESPboyInit::getKeys() { return (~mcp.readGPIOAB() & 255); }
