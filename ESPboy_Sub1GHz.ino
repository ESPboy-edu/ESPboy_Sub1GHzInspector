#include "ESPboyInit.h"
#include "ESPboyTerminalGUI.h"
#include "ESPboyMenuGUI.h"
#include "ESPboyLED.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
#include <ESP_EEPROM.h>

#define CC1101riceivePin    3
#define CC1101chipSelectPin D8
#define TFTchipSelectPin    8
#define MAX_RECORDS_TO_STORE 40


const char *menuMain[] PROGMEM = {
  "LISTEN",
  "LISTEN & STORE",
  "LIST OF STORED",
  "INFO", //Number of stored, free to store
  "CLEAR ALL", //clears all unprotected records
  "FORMAT", //clears all records
  NULL // the last element of every menu list should be NULL
};

const char *menuRecord[] PROGMEM = {
  "SEND", //send selected record
  "SHOW",
  "SET REPEAT",
  "RENAME",
  "PROTECT",
  "UNPROTECT",
  "DELETE",
  NULL
};

enum {listen_, listenstore_, listofstored_, info_, clearall_, format_};
enum {send_, show_, setrepeat_, rename_, protect_, unprotect_, delete_};

RCSwitch mySwitch;
ESPboyInit myESPboy;
ESPboyTerminalGUI *terminalGUIobj = NULL;
//ESPboyMenuGUI *menuGUIobj = NULL;
ESPboyLED myLED;

char EEPROMmagicNo[4]={0xCC,0xCD,0xCE,0};//EEPROM marker of Sub1Ghz storage

struct recordStored{
  char    recordName[20];
  uint8_t protectedFlag;
  uint8_t repeatNo;
  uint8_t protocol;
  uint32_t pulseLength;
  uint32_t Bitlength;
  uint32_t Value;
};

std::vector <recordStored> recordStoredVector;

String bin2tri(uint32_t dec) {
 uint8_t pos = 0;
 String returnVal="";
 String bin;
 
  bin = String(dec,BIN);
  while (pos < (bin.length()-1)) {
    if (bin[pos]=='0' && bin[pos+1]=='0') returnVal += '0';
    else 
      if (bin[pos]=='1' && bin[pos+1]=='1') returnVal += '1';
      else 
        if (bin[pos]=='0' && bin[pos+1]=='1') returnVal += 'F';
        else return "NO";
    pos += 2;
  }
  return returnVal;
}


void printConsoleLocal(String str, uint32_t clr, uint8_t flag1, uint8_t flag2){
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    printConsoleLocal(str, clr, flag1, flag2);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
}



void output() {
  String toPrint;
  uint32_t *rawData;
  uint32_t dec, lengthval, rssi, lqi;

  dec = mySwitch.getReceivedValue();
  lengthval = mySwitch.getReceivedBitlength();
  rssi = ELECHOUSE_cc1101.getRssi();
  lqi = ELECHOUSE_cc1101.getLqi();

  printConsoleLocal(" ", TFT_MAGENTA, 1, 1);
  printConsoleLocal(" ", TFT_MAGENTA, 1, 0);
  printConsoleLocal(F("-------------------------------"), TFT_YELLOW, 1, 0);
  printConsoleLocal(F("DETECTED! DECODE:"), TFT_RED, 1, 0);
  printConsoleLocal(F("-------------------------------"), TFT_YELLOW, 1, 0);

  //toPrint = "RSSI/LQI: ";
  //toPrint += (String)rssi;
  //toPrint += "/";
  //toPrint += (String)lqi;
  //printConsoleLocal(toPrint, TFT_YELLOW, 1, 0);
  
  toPrint="DEC: ";
  toPrint+=(String)dec;
  toPrint+=" (";
  toPrint+= (String)lengthval;
  toPrint+= " Bit)";
  printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint = String (dec,BIN);
  while(toPrint.length()<lengthval) toPrint = "0" + toPrint;
  toPrint="BIN: "+ toPrint;
  
  printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint="TRI: ";
  toPrint+= bin2tri(dec);
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  toPrint="PULSE LEN: ";
  toPrint+=(String)mySwitch.getReceivedDelay();
  toPrint+=" ms";
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);
  
  toPrint="FIRST LEV: ";
  if (mySwitch.getReceivedLevelInFirstTiming()) toPrint += "HIGH";
  else toPrint+= "LOW";
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  toPrint="PROTOCOL: ";
  toPrint+=(String)mySwitch.getReceivedProtocol();
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  toPrint="IS INV: ";
  if (mySwitch.getReceivedInverted()) toPrint+="YES";
  else toPrint+="NO";
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  printConsoleLocal(F("DATA: "), TFT_YELLOW, 1, 0);
  toPrint="";
  rawData = mySwitch.getReceivedRawdata();
  for (unsigned int i=0; i<= lengthval*2; i++) toPrint+=((String)rawData[i]+",");
  toPrint = toPrint.substring(0,toPrint.length()-1);
  printConsoleLocal(toPrint, TFT_BLUE, 1, 0);

  printConsoleLocal(F("-------------------------------"), TFT_YELLOW, 1, 0);
  printConsoleLocal("", TFT_BLACK, 1, 0);

}


void readEEPROM(){
  //EEPROM.get(0, eepromVar1);
  };


void writeEEPROM(){
 //EEPROM.put(0, eepromVar1);
 //EEPROM.commit();
  };

  

void setup(){
  EEPROM.begin(4096);
  
  myESPboy.begin("ESPboy Sub1GHz module");
  myLED.begin(&myESPboy.mcp);
  terminalGUIobj = new ESPboyTerminalGUI(&myESPboy.tft, &myESPboy.mcp);
  //menuGUIobj = new ESPboyMenuGUI(&myESPboy.tft, &myESPboy.mcp);

  delay(500);
  printConsoleLocal(F("Sub1GHz module v1.0"), TFT_MAGENTA, 1, 0);
  printConsoleLocal("", TFT_BLACK, 1, 0);
  delay(5000);
    
  ELECHOUSE_cc1101.Init();            // must be set to initialize the cc1101!
  ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setMHZ(433.92); //The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ.  
  mySwitch.enableReceive(CC1101riceivePin);  // Receiver on interrupt 0 => that is pin #2
  ELECHOUSE_cc1101.SetRx();  // set Receive on
}


void listen_f(uint8_t storeFlag){
  static String str="";
  static uint8_t ledFlag = 1;
  static uint32_t counter;


 while(!myESPboy.getKeys()){
   
  if (mySwitch.available()) {
    myLED.setRGB(0,20,0);
    ledFlag = 1;
    counter=millis();
    str=".";
    myESPboy.playTone(100,100);
    output();
    mySwitch.resetAvailable();
  }
  
  if(millis()-counter>2000){
    counter=millis();
    printConsoleLocal(str, TFT_MAGENTA, 1, 1);
    str+=".";
    if (str.length()>20) str="";
    }
    
  if(ledFlag){
    ledFlag=0;
    myLED.setRGB(0,0,0);
    printConsoleLocal(F("Listening..."), TFT_MAGENTA, 1, 0);
    printConsoleLocal(F(""), TFT_MAGENTA, 1, 0);
  }
  delay(10);
}
}


void loop(){
  listen_f(1);
 /*uint16_t menuItem;
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    delay(10);
    menuItem = menuGUIobj->menuInit(menuMain, TFT_YELLOW, TFT_BLUE, TFT_BLUE);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
  delay(10);

  
  switch (menuItem){
    case listen_:
      listen_f(0);
      break;
    case listenstore_:
      listen_f(1);
      break;
    case listofstored_:
      break;
    case info_:
      break;
    case clearall_:
      break;
    case format_:
      break;
    default:
      break;
   }*/
}
