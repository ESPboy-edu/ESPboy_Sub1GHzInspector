/*
ESPboyTerminalGUI class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v2.1
*/

//!!!!!!!!!!!!!!!!!
#define U8g2  //if defined then using font 4x6, if commented using font 6x8
//#define buttonclicks //if defined - button are clicking but it takes more than 1kb RAM, if commented - no clicks and more free RAM
//!!!!!!!!!!!!!!!!!

#ifndef ESPboy_TerminalGUI
#define ESPboy_TerminalGUI

#include "ESPboyMCP.h"
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

#define GUI_MAX_CONSOLE_STRINGS 100
#define GUI_MAX_STRINGS_ONSCREEN_FULL  ((128-2)/GUI_FONT_HEIGHT)
#define GUI_MAX_STRINGS_ONSCREEN_SMALL ((128-44)/GUI_FONT_HEIGHT)
#define GUI_MAX_TYPING_CHARS 60
#define GUI_KEY_UNPRESSED_TIMEOUT 700
#define GUI_KEY_PRESSED_DELAY_TO_SEND 500
#define GUI_CURSOR_BLINKING_PERIOD 500 
#define GUI_KEYB_CALL_DELAY 150 //auto repeat

#define GUI_PAD_LEFT        0x01
#define GUI_PAD_UP          0x02
#define GUI_PAD_DOWN        0x04
#define GUI_PAD_RIGHT       0x08
#define GUI_PAD_ACT         0x10
#define GUI_PAD_ESC         0x20
#define GUI_PAD_LFT         0x40
#define GUI_PAD_RGT         0x80
#define GUI_PAD_ANY         0xff


class ESPboyTerminalGUI{

private:
  ESPboyMCP *mcp; 
  TFT_eSPI *tft;
#ifdef U8g2
  U8g2_for_TFT_eSPI *u8f;
#endif


 struct consoleStringS {
  String consoleString;
  uint16_t consoleStringColor;
  };

 std::vector <consoleStringS> consoleStringsVector;

  struct keyboardParameters{
    int16_t renderLine;
    uint8_t displayMode;
    uint8_t shiftOn;
    int8_t selX;
    int8_t selY;
    String typing;
  }keybParam;

  const static uint8_t keybOnscr[2][3][21] PROGMEM;

  uint8_t keysAction();
  void drawConsole(uint8_t onlyLastLine);
  void drawKeyboard(uint8_t slX, uint8_t slY, uint8_t onlySelected);
  void drawBlinkingCursor();
  void drawTyping(uint8_t);
  
public:
  ESPboyTerminalGUI(TFT_eSPI *tftGUI, ESPboyMCP *mcpGUI);
  void SetKeybParamTyping(String str);
  uint8_t getKeys();
  uint32_t waitKeyUnpressed();
  void printConsole(String bfrstr, uint16_t color, uint8_t ln, uint8_t noAddLine);
  String getUserInput();
  void doScroll();
  void toggleDisplayMode(uint8_t mode);
  void drawOwnTypingLine(String typingLine, uint16_t colorLine);
};

#endif
