/*
ESPboy_MenuGUI class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v1.0
*/

//!!!!!!!!!!!!!!!!!
//#define U8g2  //if defined then using font 4x6, if commented using font 6x8
#define buttonclicks //if defined - button are clicking but it takes more than 1kb RAM, if commented - no clicks and more free RAM
////!!!!!!!!!!!!!!!!!


#ifndef ESPboy_MenuGUI
#define ESPboy_MenuGUI

#include <Adafruit_MCP23017.h>
#include <TFT_eSPI.h>
#include <FS.h> 
using fs::FS;

#ifdef U8g2
 #include "U8g2_for_TFT_eSPI.h"
#endif

#ifdef U8g2
 #define GUI_FONT_WIDTH 4
 #define GUI_FONT_HEIGHT 6
#else
 #define GUI_FONT_WIDTH 6
 #define GUI_FONT_HEIGHT 8
#endif

#define MENU_SPACE_BETWEEN_LINES (GUI_FONT_HEIGHT+3)
#define MENU_MAX_LINES_ONSCREEN (128/MENU_SPACE_BETWEEN_LINES)

#define MenuGUI_PAD_LEFT        0x01
#define MenuGUI_PAD_UP          0x02
#define MenuGUI_PAD_DOWN        0x04
#define MenuGUI_PAD_RIGHT       0x08
#define MenuGUI_PAD_ACT         0x10
#define MenuGUI_PAD_ESC         0x20
#define MenuGUI_PAD_LFT         0x40
#define MenuGUI_PAD_RGT         0x80
#define MenuGUI_PAD_ANY         0xff


class ESPboyMenuGUI{

private:
  Adafruit_MCP23017 *mcp; 
  TFT_eSPI *tft;
#ifdef U8g2
  U8g2_for_TFT_eSPI *u8f;
#endif

struct menuStruct{
  const char **menuLine;
  uint16_t menuOffset;
  uint16_t menuItemsQuantity;
  uint16_t menuLineColor;
  uint16_t menuUnselectedLineColor;
  uint16_t menuSelectionColor;
  uint16_t menuCurrent;
} menuList;

uint8_t getKeys();
void menuDraw();
  
public:
  ESPboyMenuGUI(TFT_eSPI *tftMenuGUI, Adafruit_MCP23017 *mcpMenuGUI);
  uint16_t menuInit(const char** menuLinesF, uint16_t menuLineColorF, uint16_t menuUnselectedLineColorF, uint16_t menuSelectionColorF);
};

#endif
