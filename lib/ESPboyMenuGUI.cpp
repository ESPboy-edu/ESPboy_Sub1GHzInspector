/*
ESPboy_MenuGUI class
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v1.0
*/

#include "ESPboyMenuGUI.h"

ESPboyMenuGUI::ESPboyMenuGUI(ESPboyInit *myESPboyPointer) {
   myESPboy = myESPboyPointer;
#ifdef U8g2
   u8f = new U8g2_for_TFT_eSPI;
   u8f->begin(myESPboy->tft); 
   u8f->setFontMode(1);                 // use u8g2 none transparent mode
   u8f->setBackgroundColor(TFT_BLACK);
   u8f->setFontDirection(0);            // left to right
   u8f->setFont(u8g2_font_4x6_t_cyrillic); 
#endif
}



void ESPboyMenuGUI::menuDraw(){
  static uint16_t scalingFactor;
  static uint16_t previousRect = 0;

  if(menuList.menuItemsQuantity>1){
    if(menuList.menuItemsQuantity>=MENU_MAX_LINES_ONSCREEN)
      scalingFactor = ((MENU_MAX_LINES_ONSCREEN*MENU_SPACE_BETWEEN_LINES-6)*1000)/(menuList.menuItemsQuantity-1);
    else
      scalingFactor = ((menuList.menuItemsQuantity*MENU_SPACE_BETWEEN_LINES-6)*1000)/(menuList.menuItemsQuantity-1);
  }
  else scalingFactor=1;
  
  myESPboy->tft.drawRect(0, previousRect*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, TFT_BLACK);
  myESPboy->tft.fillRect(125,0, 3, 128, TFT_BLACK);

  if (menuList.menuCurrent+1 > MENU_MAX_LINES_ONSCREEN + menuList.menuOffset) {
      myESPboy->tft.fillScreen(TFT_BLACK); 
      menuList.menuOffset++;}
  if (menuList.menuCurrent < menuList.menuOffset) {
      myESPboy->tft.fillScreen(TFT_BLACK); 
      menuList.menuOffset--;}

  if(menuList.menuItemsQuantity>=MENU_MAX_LINES_ONSCREEN)
    myESPboy->tft.drawLine(126,0, 126, MENU_MAX_LINES_ONSCREEN*MENU_SPACE_BETWEEN_LINES-2, TFT_BLUE);
  else
    myESPboy->tft.drawLine(126,0, 126, menuList.menuItemsQuantity*MENU_SPACE_BETWEEN_LINES-2, TFT_BLUE);
  
  for (uint8_t i=0;; i++){
    if(i>=menuList.menuItemsQuantity || i>=MENU_MAX_LINES_ONSCREEN) break;

    #ifndef U8g2
      if(menuList.menuLine[i+menuList.menuOffset][0] == '-'){ 
        myESPboy->tft.setTextColor(menuList.menuUnselectedLineColor);
        myESPboy->tft.drawString(&menuList.menuLine[i+menuList.menuOffset][1], 3, i*MENU_SPACE_BETWEEN_LINES+2);
      }
      else{
        myESPboy->tft.setTextColor(menuList.menuLineColor);
        myESPboy->tft.drawString(menuList.menuLine[i+menuList.menuOffset], 3, i*MENU_SPACE_BETWEEN_LINES+2); 
      }
    #else
      if(menuList.menuLine[i+menuList.menuOffset][0] == '-'){ 
        u8f->setForegroundColor(menuList.menuUnselectedLineColor);
        u8f->drawStr(3, i*MENU_SPACE_BETWEEN_LINES+1+GUI_FONT_HEIGHT, &menuList.menuLine[i+menuList.menuOffset][1]);
      }
      else { 
        u8f->setForegroundColor(menuList.menuLineColor);
        u8f->drawStr(3, i*MENU_SPACE_BETWEEN_LINES+1+GUI_FONT_HEIGHT, menuList.menuLine[i+menuList.menuOffset]);
      }
    #endif
    
    if((i+menuList.menuOffset) == menuList.menuCurrent){
      myESPboy->tft.drawRect(0, i*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, menuList.menuSelectionColor);
      previousRect=i;}
  }

 myESPboy->tft.fillRect(125, (scalingFactor*menuList.menuCurrent+2)/1000, 3, 5, TFT_YELLOW);
}



uint16_t ESPboyMenuGUI::menuInit(const char** menuLinesF, uint16_t menuLineColorF, uint16_t menuUnselectedLineColorF, uint16_t menuSelectionColorF){             
 uint16_t count=0;
 static uint8_t keyPressed;
  myESPboy->tft.fillScreen(TFT_BLACK);
  menuList.menuLine = menuLinesF;
  menuList.menuLineColor = menuLineColorF;
  menuList.menuUnselectedLineColor = menuUnselectedLineColorF;
  menuList.menuCurrent = 0;
  menuList.menuSelectionColor = menuSelectionColorF;
  menuList.menuOffset=0;
  while(menuLinesF[count++]);
  menuList.menuItemsQuantity = count-1;

  menuDraw();

 while(1){  
  while (!myESPboy->getKeys())delay(50);
  
  keyPressed = myESPboy->getKeys();
  
  if (keyPressed&MenuGUI_PAD_UP && menuList.menuCurrent > 0) {
    menuList.menuCurrent--;
    #ifdef buttonclicks
    myESPboy->playTone(10,10);
    #endif
    menuDraw();
  }
  if (keyPressed&MenuGUI_PAD_DOWN && menuList.menuCurrent+1 < menuList.menuItemsQuantity) {
    menuList.menuCurrent++;
    #ifdef buttonclicks
    myESPboy->playTone(10,10);
    #endif
    menuDraw();
  }
  if (keyPressed&MenuGUI_PAD_ACT && menuList.menuLine[menuList.menuCurrent][0] != '-') {
    #ifdef buttonclicks  
      myESPboy->playTone(100,100);
    #endif
    
    myESPboy->tft.drawRect(0, (menuList.menuCurrent+menuList.menuOffset)*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, TFT_BLACK);
    delay(50);
    myESPboy->tft.drawRect(0, (menuList.menuCurrent+menuList.menuOffset)*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, menuList.menuSelectionColor);
    delay(50);
    myESPboy->tft.drawRect(0, (menuList.menuCurrent+menuList.menuOffset)*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, TFT_BLACK);
    delay(50);
    myESPboy->tft.drawRect(0, (menuList.menuCurrent+menuList.menuOffset)*MENU_SPACE_BETWEEN_LINES, 122, MENU_SPACE_BETWEEN_LINES, menuList.menuSelectionColor);
    delay(200);
    
    myESPboy->tft.fillScreen(TFT_BLACK);
    return(menuList.menuCurrent+1);
  }
  if (keyPressed&MenuGUI_PAD_ESC){
    #ifdef buttonclicks  
      myESPboy->playTone(100,100);
    #endif    
    myESPboy->tft.fillScreen(TFT_BLACK);
    return(0);
  }
  
  delay(120);
 }
};
