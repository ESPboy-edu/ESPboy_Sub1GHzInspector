/*
ESPboyTerminalGUI class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v2.1
*/

#include "ESPboyTerminalGUI.h"
#define SOUNDPIN D3

const uint8_t ESPboyTerminalGUI::keybOnscr[2][3][21] PROGMEM = {
 {"+1234567890abcdefghi", "jklmnopqrstuvwxyz -=", "?!@$%&*()_[]\":;.,^<E",},
 {"+1234567890ABCDEFGHI", "JKLMNOPQRSTUVWXYZ -=", "?!@$%&*()_[]\":;.,^<E",}
};


ESPboyTerminalGUI::ESPboyTerminalGUI(TFT_eSPI *tftGUI, Adafruit_MCP23017 *mcpGUI) {
   keybParam.renderLine = 0;
   keybParam.displayMode = 0;
   keybParam.shiftOn = 0;
   keybParam.selX = 0;
   keybParam.selY = 0;
   keybParam.typing = "";
   
   tft = tftGUI;
   mcp = mcpGUI;
#ifdef U8g2
   u8f = new U8g2_for_TFT_eSPI;
   u8f->begin(*tft); 
   u8f->setFontMode(1);                 // use u8g2 none transparent mode
   u8f->setBackgroundColor(TFT_BLACK);
   u8f->setFontDirection(0);            // left to right
   u8f->setFont(u8g2_font_4x6_t_cyrillic); 
#endif
   consoleStringsVector.push_back(consoleStringS());
   consoleStringsVector.back().consoleString = "";
   consoleStringsVector.back().consoleStringColor = TFT_BLACK;
   toggleDisplayMode(1);
}


uint8_t ESPboyTerminalGUI::keysAction() {
  uint8_t longActPress = 0;
  uint8_t keyState = getKeys();

  if (keyState) {
  #ifdef buttonclicks
    tone(SOUNDPIN, 100, 10);
  #endif
    if (!keybParam.displayMode) {
      if (keyState & GUI_PAD_LEFT && keyState & GUI_PAD_UP) {  // shift
        keybParam.shiftOn = !keybParam.shiftOn;
        drawKeyboard(keybParam.selX, keybParam.selY, 0);
        waitKeyUnpressed();
      } else {
        if ((keyState & GUI_PAD_RIGHT) && keybParam.selX < 20) keybParam.selX++;
        if ((keyState & GUI_PAD_LEFT) && keybParam.selX > -1) keybParam.selX--;
        if ((keyState & GUI_PAD_DOWN) && keybParam.selY < 3) keybParam.selY++;
        if ((keyState & GUI_PAD_UP) && keybParam.selY > -1) keybParam.selY--;
        if ((keyState & GUI_PAD_LEFT) && keybParam.selX == -1) keybParam.selX = 19;
        if ((keyState & GUI_PAD_RIGHT) && keybParam.selX == 20) keybParam.selX = 0;
        if ((keyState & GUI_PAD_UP) && keybParam.selY == -1) keybParam.selY = 2;
        if ((keyState & GUI_PAD_DOWN) && keybParam.selY == 3) keybParam.selY = 0;
      }

      if ((keyState&GUI_PAD_ACT && keyState&GUI_PAD_ESC) || (keyState&GUI_PAD_RGT && keyState&GUI_PAD_LFT)) {
        if (keybParam.renderLine > consoleStringsVector.size() - GUI_MAX_STRINGS_ONSCREEN_FULL)
          keybParam.renderLine = consoleStringsVector.size() - GUI_MAX_STRINGS_ONSCREEN_FULL;
        toggleDisplayMode(1);
        waitKeyUnpressed();
        
      } else if (keyState&GUI_PAD_RGT && keybParam.renderLine) {
        keybParam.renderLine--;
        drawConsole(0);
        
      } else if (keyState&GUI_PAD_LFT && keybParam.renderLine < consoleStringsVector.size() - GUI_MAX_STRINGS_ONSCREEN_SMALL) {
        keybParam.renderLine++;
        drawConsole(0);
      }

      if ((((keyState & GUI_PAD_ACT) && (keybParam.selX == 19 && keybParam.selY == 2)) || (keyState & GUI_PAD_RGT && keyState & GUI_PAD_LFT))) {  // enter
        if (keybParam.typing.length() > 0) longActPress = 1;
      } else if ((keyState & GUI_PAD_ACT) && (keybParam.selX == 18 && keybParam.selY == 2)) {  // back space
        if (keybParam.typing.length() > 0) keybParam.typing.remove(keybParam.typing.length() - 1);
      } else if ((keyState & GUI_PAD_ACT) && (keybParam.selX == 17 && keybParam.selY == 1)) {  // SPACE
            if (keybParam.typing.length() < GUI_MAX_TYPING_CHARS) keybParam.typing += " ";
      } else if ((keyState & GUI_PAD_ACT) && (keybParam.selX == 17 && keybParam.selY == 2)) {
        keybParam.shiftOn = !keybParam.shiftOn;
        drawKeyboard(keybParam.selX, keybParam.selY, 0);
        waitKeyUnpressed();
      } else if (keyState & GUI_PAD_ACT){
        if (waitKeyUnpressed() > GUI_KEY_PRESSED_DELAY_TO_SEND)
          longActPress = 1;
        else if (keybParam.typing.length() < GUI_MAX_TYPING_CHARS)
          keybParam.typing += (char)pgm_read_byte(&keybOnscr[keybParam.shiftOn][keybParam.selY][keybParam.selX]);
      }

      if (keyState & GUI_PAD_ESC) {
        if (waitKeyUnpressed() > GUI_KEY_PRESSED_DELAY_TO_SEND)
          keybParam.typing = "";
        else if (keybParam.typing.length() > 0)
          keybParam.typing.remove(keybParam.typing.length() - 1);
      }
    }

    else {
      if ((keyState & GUI_PAD_ACT && keyState & GUI_PAD_ESC) || (keyState & GUI_PAD_RGT && keyState & GUI_PAD_LFT)) {
        toggleDisplayMode(0);
        waitKeyUnpressed();
      } else

          if (((keyState & GUI_PAD_RGT || keyState & GUI_PAD_RIGHT || keyState & GUI_PAD_DOWN)) && keybParam.renderLine > 0) {
            keybParam.renderLine--;
            drawConsole(0);
          } else

          if (((keyState&GUI_PAD_LFT || keyState & GUI_PAD_LEFT || keyState&GUI_PAD_UP)) && keybParam.renderLine < consoleStringsVector.size() - GUI_MAX_STRINGS_ONSCREEN_FULL) {
        keybParam.renderLine++;
        drawConsole(0);
      } else

          if (keyState & GUI_PAD_ESC)
            toggleDisplayMode(0);
    }
    if (!keybParam.displayMode) drawKeyboard(keybParam.selX, keybParam.selY, 1);
  }

  if (!keybParam.displayMode) drawBlinkingCursor();
  return (longActPress);
}

void ESPboyTerminalGUI::toggleDisplayMode(uint8_t mode) {
  keybParam.displayMode = mode;
  tft->fillScreen(TFT_BLACK);
  tft->drawRect(0, 0, 128, 128, TFT_NAVY);
  if (!keybParam.displayMode) {
    tft->drawRect(0, 128 - 3 * 8 - 5, 128, 3 * 8 + 5, TFT_NAVY);
    tft->drawRect(0, 0, 128, 86, TFT_NAVY);
  }
  if (!keybParam.displayMode) {
    drawKeyboard(keybParam.selX, keybParam.selY, 0);
  }
  drawConsole(0);
}

String ESPboyTerminalGUI::getUserInput() {
  String userInput;
  toggleDisplayMode(0);
  while (1) {
    while (!keysAction()) delay(GUI_KEYB_CALL_DELAY);
    if (keybParam.typing != "") break;
  }
  toggleDisplayMode(1);
  userInput = keybParam.typing;
  keybParam.typing = "";
  return (userInput);
}

void ESPboyTerminalGUI::printConsole(String bfrstr, uint16_t color, uint8_t ln, uint8_t noAddLine) {
  String toprint;
  
  keybParam.renderLine = 0;

  if(bfrstr == "") bfrstr = " ";
  
  if (!ln)
    if (bfrstr.length() > ((128-4)/GUI_FONT_WIDTH)) {
      bfrstr = bfrstr.substring(0, ((128-4)/GUI_FONT_WIDTH));
      toprint = bfrstr;
  }

  for (uint8_t i = 0; i <= ((bfrstr.length()-1) / ((128-4)/GUI_FONT_WIDTH)); i++) {
    toprint = bfrstr.substring(i * (128-4)/GUI_FONT_WIDTH);
    toprint = toprint.substring(0, (128-4)/GUI_FONT_WIDTH);

    if (!noAddLine) consoleStringsVector.push_back(consoleStringS());
    consoleStringsVector.back().consoleString = toprint;
    consoleStringsVector.back().consoleStringColor = color;
  }
  
  drawConsole(noAddLine);
  
  if (consoleStringsVector.size() > GUI_MAX_CONSOLE_STRINGS){
    consoleStringsVector.erase(consoleStringsVector.begin());
    consoleStringsVector.shrink_to_fit();
  }
}



void ESPboyTerminalGUI::drawConsole(uint8_t onlyLastLine) {
  uint16_t lines;
  uint16_t offsetY;
  uint16_t quantityLinesToDraw;
  int16_t startVectorToDraw;
  
  if (keybParam.displayMode) lines = GUI_MAX_STRINGS_ONSCREEN_FULL;
  else lines = GUI_MAX_STRINGS_ONSCREEN_SMALL;


#ifndef U8g2
  if (!onlyLastLine) tft->fillRect(1, 4, 126, lines * GUI_FONT_HEIGHT, TFT_BLACK);
  else tft->fillRect(1, (lines-1) * GUI_FONT_HEIGHT+4, 126, GUI_FONT_HEIGHT, TFT_BLACK);
#else
  if (!onlyLastLine) tft->fillRect(1, 1, 126, lines * GUI_FONT_HEIGHT, TFT_BLACK);
  else tft->fillRect(1, (lines-1) * GUI_FONT_HEIGHT, 126, GUI_FONT_HEIGHT+1, TFT_BLACK);
#endif  


#ifndef U8g2
  offsetY = lines * GUI_FONT_HEIGHT - 4;
#else
  offsetY = lines * GUI_FONT_HEIGHT;
#endif  

  if(consoleStringsVector.size() < lines ) quantityLinesToDraw = consoleStringsVector.size();
  else quantityLinesToDraw = lines;

  startVectorToDraw = consoleStringsVector.size()-1-keybParam.renderLine;
  
  for (uint8_t i = 0; i< quantityLinesToDraw; i++) {
#ifndef U8g2
    tft->setTextColor(consoleStringsVector[startVectorToDraw].consoleStringColor, TFT_BLACK);
    tft->drawString(consoleStringsVector[startVectorToDraw].consoleString, 3, offsetY);
#else
    u8f->setForegroundColor(consoleStringsVector[startVectorToDraw].consoleStringColor);
    u8f->drawStr(2, offsetY, consoleStringsVector[startVectorToDraw].consoleString.c_str());
#endif
    offsetY -= GUI_FONT_HEIGHT;
    startVectorToDraw--;
    if(startVectorToDraw<0) startVectorToDraw=0;
  } 
}


uint8_t ESPboyTerminalGUI::getKeys() { return (~mcp->readGPIOAB() & 255); }


uint32_t ESPboyTerminalGUI::waitKeyUnpressed() {
  uint32_t timerStamp = millis();
  while (getKeys() && (millis() - timerStamp) < GUI_KEY_UNPRESSED_TIMEOUT) delay(1);
  return (millis() - timerStamp);
}


void ESPboyTerminalGUI::drawKeyboard(uint8_t slX, uint8_t slY, uint8_t onlySelected) {
  static char chr[2]={0,0};
  static uint8_t prevX = 0, prevY = 0;

  if (!onlySelected) {
    tft->fillRect(1, 128 - 24, 126, 23, TFT_BLACK);
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    for (uint8_t j = 0; j < 3; j++)
      for (uint8_t i = 0; i < 20; i++) {
        chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][j][i]);
        tft->drawString(&chr[0], i * 6 + 4, 128 - 2 - 8 * (3 - j));
      }
  }

  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][prevY][prevX]);
  tft->drawString(&chr[0], prevX * 6 + 4, 128 - 24 + prevY * 8 - 2);

  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->drawString("^<E", 6 * 17 + 4, 128 - 24 + 2 * 8 - 2);

  tft->setTextColor(TFT_YELLOW, TFT_RED);
  chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][slY][slX]);
  tft->drawString(&chr[0], slX * 6 + 4, 128 - 24 + slY * 8 - 2);
  
  prevX = slX;
  prevY = slY;

  drawTyping(0);
}


void ESPboyTerminalGUI::drawTyping(uint8_t changeCursor) {
  static char cursorType[2] = {220, '_'};
  static uint8_t cursorTypeFlag=0;

  if(changeCursor) cursorTypeFlag=!cursorTypeFlag;
  tft->fillRect(1, 128 - 5 * 8, 126, 10, TFT_BLACK);
  if (keybParam.typing.length() < 20) {
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString(keybParam.typing + cursorType[cursorTypeFlag], 4, 128 - 5 * 8 + 1);
  } else {
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString("<" + keybParam.typing.substring(keybParam.typing.length() - 18) +cursorType[cursorTypeFlag], 4, 128 - 5 * 8 + 1);
  }
}


void ESPboyTerminalGUI::drawOwnTypingLine(String typingLine, uint16_t colorLine){
  keybParam.typing = typingLine;
  toggleDisplayMode(0);
  keybParam.typing = typingLine;
  tft->fillRect(1, 128 - 5 * 8, 126, 10, TFT_BLACK);
  tft->setTextColor(colorLine, TFT_BLACK);
  tft->drawString(keybParam.typing, 4, 128 - 5 * 8 + 1);
}



void ESPboyTerminalGUI::drawBlinkingCursor() {
 static uint32_t cursorBlinkMillis = 0; 
  if (millis() > (cursorBlinkMillis + GUI_CURSOR_BLINKING_PERIOD)) {
    cursorBlinkMillis = millis();
    drawTyping(1);
  }
}


void ESPboyTerminalGUI::SetKeybParamTyping(String str){
  keybParam.typing = str;
}
