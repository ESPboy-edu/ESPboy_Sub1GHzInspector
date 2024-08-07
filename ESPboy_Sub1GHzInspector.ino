/*
ESPboy Sub1Ghz inspector
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
v2.0

thanks to Gerashka for update protocols and lib modding https://github.com/gerashk/ESPboy_Sub1GHz_Mod
*/

#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"
#include "lib/ESPboyTerminalGUI.h"
#include "lib/ESPboyTerminalGUI.cpp"
#include "lib/ESPboyMenuGUI.h"
#include "lib/ESPboyMenuGUI.cpp"
//#include "lib/ESPboyOTA2.h"
//#include "lib/ESPboyOTA2.cpp"

#include "lib/SmartRC-CC1101-Driver-Lib-master/ELECHOUSE_CC1101_SRC_DRV.h"
#include "lib/SmartRC-CC1101-Driver-Lib-master/ELECHOUSE_CC1101_SRC_DRV.cpp"
#include "lib/rc-switch_mod/src/RCSwitch.h"
#include "lib/rc-switch_mod/src/RCSwitch.cpp"
#include <ESP_EEPROM.h>

#define CC1101riceivePin    3
#define CC1101sendPin       2
#define CC1101chipSelectPin D8
#define TFTchipSelectPin    8

#define APP_ID 0xCCCD
#define MAX_RECORDS_TO_STORE 40
#define DEFAULT_SIGNAL_REPEAT_NUMBER 3
#define EEPROM_OFFSET 10



String protDecode[]={
"Unknown",
"Protocol 01 Princeton, PT-2240",
"Protocol 02 AT-Motor?",
"Protocol 03",
"Protocol 04",
"Protocol 05",
"Protocol 06 HT6P20B",
"Protocol 07 HS2303-PT, i. e. used in AUKEY Remote",
"Protocol 08 Came 12bit, HT12E",
"Protocol 09 Nice_Flo 12bit",
"Protocol 10 V2 phoenix",
"Protocol 11 Nice_FloR-S 52bit",
"Protocol 12 Keeloq 64/66 falseok",
"Protocol 13 test CFM",
"Protocol 14 test StarLine",
"Protocol 15",
"Protocol 16 Einhell",
"Protocol 17 InterTechno PAR-1000",
"Protocol 18 Intertechno ITT-1500",
"Protocol 19 Murcury",
"Protocol 20 AC114",
"Protocol 21 DC250",
"Protocol 22 Mandolyn/Lidl TR-502MSV/RC-402/RC-402DX",
"Protocol 23 Lidl TR-502MSV/RC-402 - Flavien",
"Protocol 24 Lidl TR-502MSV/RC701",
"Protocol 25 NEC",
"Protocol 26 Arlec RC210",
"Protocol 27 Zap, FHT-7901",
"Protocol 28", // github.com/sui77/rc-switch/pull/115
"Protocol 29 NEXA",
"Protocol 30 Anima",
"Protocol 31 Mertik Maxitrol G6R-H4T1",
"Protocol 32", //github.com/sui77/rc-switch/pull/277
"Protocol 33 Dooya Control DC2708L",
"Protocol 34 DIGOO SD10 ", //so as to use this protocol RCSWITCH_SEPARATION_LIMIT must be set to 2600
"Protocol 35 Dooya 5-Channel blinds remote DC1603",
"Protocol 36 DC2700AC", //Dooya remote DC2700AC for Dooya DT82TV curtains motor
"Protocol 37 DEWENWILS Power Strip",
"Protocol 38 Nexus weather, 36 bit",
"Protocol 39 Louvolite with premable"
};


char *menuList[MAX_RECORDS_TO_STORE+1];

const char *menuMain[] PROGMEM = {
  "LISTEN",
  "LISTEN & STORE",
  "LIST OF STORED",
  "SAVE ALL",
  "CLEAR ALL", //clears all unprotected records
  NULL // the last element of every menu list should be NULL
};

const char *menuRecord[] PROGMEM = {
  "SEND", //send selected record
  "SHOW",
  "SET REPEAT",
  "RENAME",
  "DELETE",
  "CANCEL",
  NULL
};

enum {listen_=1, listenstore_, listofstored_, saveall_, clearall_};
enum {send_=1, show_, setrepeat_, rename_, delete_, cancel_};

RCSwitch mySwitch;
ESPboyInit myESPboy;
ESPboyTerminalGUI *terminalGUIobj = NULL;
ESPboyMenuGUI *menuGUIobj = NULL;
//ESPboyOTA2 *OTA2obj = NULL;

struct EEPROMinitStruct{
 uint32_t appId = APP_ID;
 uint8_t  recordsQuantity = 0;
}EEPROMinit;

struct recordStored{
  char     recordName[20];
  uint32_t recordRepeatno;
  uint32_t recordProtocol;
  uint32_t recordPulselen;
  uint32_t recordBits;
  uint32_t recordValue;
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
    terminalGUIobj->printConsole(str, clr, flag1, flag2);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
}


String getUserInputLocal(){
  String inputLocal;
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    inputLocal = terminalGUIobj->getUserInput();
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
  return (inputLocal);
}



void toggleDisplayModeLocal(uint8_t displayMode){
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    terminalGUIobj->toggleDisplayMode(displayMode);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
}


void doScrollLocal(){
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    terminalGUIobj->doScroll();
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
}


uint16_t menuInitLocal(const char** menuLines, uint16_t menuLineColor, uint16_t menuUnselectedLineColor, uint16_t menuSelectionColor){
  uint16_t menuItemSelected; 
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  digitalWrite(CC1101chipSelectPin, HIGH);
    menuItemSelected = menuGUIobj->menuInit(menuLines, menuLineColor, menuUnselectedLineColor, menuSelectionColor);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
  return (menuItemSelected);
}


void drawDecodedSignal() {
  String toPrint;
  uint32_t *rawData;
  uint32_t dec, lengthval, rssi, lqi;

  int16_t databuffer[64]; // get a copy of the received timings before they are overwritten
  int16_t numberoftimings = 2 * mySwitch.getReceivedBitlength() + 2;
  if(numberoftimings > 64) numberoftimings = 64;
  for (int8_t i = 0; i < numberoftimings; i++)
    databuffer[i] = mySwitch.getReceivedRawdata()[i];
  
  dec = mySwitch.getReceivedValue();
  lengthval = mySwitch.getReceivedBitlength();
  rssi = ELECHOUSE_cc1101.getRssi();
  lqi = ELECHOUSE_cc1101.getLqi();

  printConsoleLocal(" ", TFT_MAGENTA, 1, 1);
  printConsoleLocal(" ", TFT_MAGENTA, 1, 0);
  printConsoleLocal(F("DETECTED! DECODE:"), TFT_RED, 1, 0);

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
  printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint="PROTOCOL: ";
  toPrint+=(String)mySwitch.getReceivedProtocol();
  printConsoleLocal(toPrint, TFT_YELLOW, 1, 0);
  printConsoleLocal(protDecode[mySwitch.getReceivedProtocol()], TFT_YELLOW, 1, 0);

  uint16_t databitsoffset = 0;//abs((int16_t)mySwitch.getReceivedLevelInFirstTiming() - (int16_t)mySwitch.getReceivedInverted());
  uint32_t dataduration = 0;
  for (uint16_t i = 1 + databitsoffset; i < numberoftimings - 1 + databitsoffset; i++)
    dataduration += databuffer[i];

  uint16_t averagebitduration = (int16_t)(0.5 + ((double)dataduration)/mySwitch.getReceivedBitlength());
  uint16_t protocolratio = (uint16_t)(0.5 + ((double)(averagebitduration - mySwitch.getReceivedDelay())) / (double)mySwitch.getReceivedDelay());

  toPrint = mySwitch.getReceivedDelay();
  toPrint+=" {";
  toPrint+= (databitsoffset==0) ? 
     (int16_t) (0.5 + (double)databuffer[2*mySwitch.getReceivedBitlength()+1]/(double)mySwitch.getReceivedDelay()):
     (int16_t) (0.5 + (double)databuffer[0]/(double)mySwitch.getReceivedDelay());
  toPrint+=" ";
  toPrint+= (databitsoffset==0) ?
     (int16_t) (0.5 + (double)databuffer[0]/(double)mySwitch.getReceivedDelay()):
     (int16_t) (0.5 + (double)databuffer[1]/(double)mySwitch.getReceivedDelay());
  toPrint+="} {";
  toPrint+="1";
  toPrint+=" ";
  toPrint+=protocolratio;
  toPrint+= "} ";
  //toPrint+=(mySwitch.getReceivedInverted()) ? "true" : "false";
  if(!mySwitch.getReceivedProtocol())printConsoleLocal(toPrint, TFT_RED, 1, 0);
  else printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint="PULSE LEN: ";
  toPrint+=(String)mySwitch.getReceivedDelay();
  toPrint+=" ms";
  printConsoleLocal(toPrint, TFT_WHITE, 1, 0);
  
  //toPrint="FIRST LEV: ";
  //if (mySwitch.getReceivedLevelInFirstTiming()) toPrint += "HIGH";
  //else toPrint+= "LOW";
  //printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  //toPrint="IS INV: ";
  //if (mySwitch.getReceivedInverted()) toPrint+="TRUE";
  //else toPrint+="FALSE";
  //printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

  printConsoleLocal(F("DATA: "), TFT_BLUE, 1, 0);
  toPrint="";
  rawData = mySwitch.getReceivedRawdata();
  for (unsigned int i=0; i<= lengthval*2; i++) toPrint+=((String)rawData[i]+",");
  toPrint = toPrint.substring(0,toPrint.length()-1);
  printConsoleLocal(toPrint, TFT_BLUE, 1, 0);

  printConsoleLocal("", TFT_BLACK, 1, 0);

}



void readEEPROM(){  
 EEPROM.get(EEPROM_OFFSET, EEPROMinit);
 if (EEPROMinit.appId != APP_ID){
   EEPROMinit.appId = APP_ID;
   EEPROMinit.recordsQuantity = 0;
   EEPROM.put(EEPROM_OFFSET, EEPROMinit);
   EEPROM.commit();
 }
 
 for (uint8_t i=0; i<EEPROMinit.recordsQuantity; i++){
      recordStoredVector.push_back(recordStored());
      EEPROM.get(EEPROM_OFFSET + sizeof(EEPROMinit) + i*(sizeof(recordStored)), recordStoredVector.back());
 };
};



void writeEEPROMall(){
  EEPROMinit.recordsQuantity = recordStoredVector.size();
  EEPROM.put(EEPROM_OFFSET, EEPROMinit);
  for (uint8_t i=0; i<EEPROMinit.recordsQuantity; i++)
    EEPROM.put(EEPROM_OFFSET + sizeof(EEPROMinit) + i*(sizeof(recordStored)), recordStoredVector[i]);
  EEPROM.commit();
};



void storeSignal(){
  String signalName;
  
  recordStoredVector.push_back(recordStored());

  recordStoredVector.back().recordRepeatno = DEFAULT_SIGNAL_REPEAT_NUMBER;
  recordStoredVector.back().recordProtocol = mySwitch.getReceivedProtocol();
  recordStoredVector.back().recordPulselen = mySwitch.getReceivedDelay();
  recordStoredVector.back().recordBits = mySwitch.getReceivedBitlength();
  recordStoredVector.back().recordValue = mySwitch.getReceivedValue();

  signalName = "(";
  signalName += (String)mySwitch.getReceivedBitlength();
  signalName += " bit) ";
  signalName = signalName.substring(0,18);
  signalName += (String)mySwitch.getReceivedValue();
  memcpy(&recordStoredVector.back().recordName, signalName.c_str(), signalName.length()+1);
};


void setup(){
  EEPROM.begin(2048);
  readEEPROM();
  
  myESPboy.begin("Sub1GHz inspector");

  //Check OTA2
/*
  if (myESPboy.getKeys()&PAD_ACT || myESPboy.getKeys()&PAD_ESC) { 
     terminalGUIobj = new ESPboyTerminalGUI(&myESPboy.tft, &myESPboy.mcp);
     OTA2obj = new ESPboyOTA2(terminalGUIobj);
  }
*/
  
  terminalGUIobj = new ESPboyTerminalGUI(&myESPboy.tft, &myESPboy.mcp);
  menuGUIobj = new ESPboyMenuGUI(&myESPboy);

  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);

    // if return 0 then CC1101 is connected
  if(ELECHOUSE_cc1101.getCC1101()){
    myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
    digitalWrite(CC1101chipSelectPin, HIGH);
    myESPboy.tft.setTextColor(0xF800);
    myESPboy.tft.drawString("CC1101 module", 28 ,40);
    myESPboy.tft.drawString("not found", 37 ,55);
    while(1) delay(100);
  }
    
  ELECHOUSE_cc1101.Init();            // must be set to initialize the cc1101!
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setMHZ(433.92); //The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ.  
  mySwitch.enableReceive(CC1101riceivePin);  // Receiver on interrupt 0 => that is pin #2
  ELECHOUSE_cc1101.SetRx();  // set Receive on
}



void listen_f(uint8_t storeFlag){
  String str="";
  uint8_t ledFlag = 1;
  uint32_t counter;

 mySwitch.resetAvailable();
 toggleDisplayModeLocal(1); 
 
 while(!(myESPboy.getKeys()&PAD_ESC)){
  uint8_t pressedButtons = myESPboy.getKeys();
  if(pressedButtons && !(pressedButtons&PAD_RGT) && !(pressedButtons&PAD_LFT)){
    printConsoleLocal(F("Stop"), TFT_MAGENTA, 1, 1);
    printConsoleLocal(F("LGT/RGT-scroll. B-exit"), TFT_MAGENTA, 1, 0);
    doScrollLocal();
    ledFlag = 1;
    str="";
  } 
  
  if (mySwitch.available()) {
    myESPboy.myLED.setRGB(0,20,0);
    ledFlag = 1;
    counter=millis();
    str="";
    myESPboy.playTone(100,100);
    drawDecodedSignal();
    
 if (storeFlag){
   if (!mySwitch.getReceivedProtocol()){
      printConsoleLocal(F("Can't store UNKNOWN PROTOCOL"), TFT_RED, 1, 0);}
   else
     if(recordStoredVector.size()<MAX_RECORDS_TO_STORE){
       storeSignal();
       String toPrint = F("Signal stored. TOTAL: ");
       toPrint += String (recordStoredVector.size());
       toPrint +=F(" of ");
       toPrint += (String)MAX_RECORDS_TO_STORE;
       printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);}
     else{
      String toPrint = F("Can't store. Max reached ");
      toPrint += (String)MAX_RECORDS_TO_STORE;
      printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);}
     }   
    mySwitch.resetAvailable();
  }
  
  if(millis()-counter>2000){
    myESPboy.myLED.setRGB(0,20,0);
    counter=millis();
    printConsoleLocal(str, TFT_MAGENTA, 1, 1);
    str+=".";
    if (str.length()>20) str="";
    myESPboy.myLED.setRGB(0,0,0);
    }
    
  if(ledFlag){
    ledFlag=0;
    myESPboy.myLED.setRGB(0,0,0);
    printConsoleLocal(F("Listening..."), TFT_MAGENTA, 1, 0);
    printConsoleLocal(F(""), TFT_MAGENTA, 1, 0);
  }
  delay(10);
 }

printConsoleLocal(F("Stop. Exit"), TFT_MAGENTA, 1, 1);
printConsoleLocal("", TFT_MAGENTA, 1, 0);
while(myESPboy.getKeys()) delay(10);
}


void send_f(uint16_t selectedSignal){
  myESPboy.myLED.setRGB(20,0,0);
  selectedSignal--;
  toggleDisplayModeLocal(1);
  String toPrint = F("Sending ");
  toPrint += recordStoredVector[selectedSignal].recordName;
  printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
  toPrint = (String)recordStoredVector[selectedSignal].recordRepeatno;
  toPrint += " times";
  printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
    
  ELECHOUSE_cc1101.SetTx();           // set Transmit on
  mySwitch.disableReceive();         // Receiver off
  mySwitch.enableTransmit(CC1101sendPin);   // Transmit on
  mySwitch.setRepeatTransmit(recordStoredVector[selectedSignal].recordRepeatno); // transmission repetitions.
  mySwitch.setProtocol(recordStoredVector[selectedSignal].recordProtocol);        // send Received Protocol
  mySwitch.setPulseLength(recordStoredVector[selectedSignal].recordPulselen);    // send Received Delay
  mySwitch.send(recordStoredVector[selectedSignal].recordValue,recordStoredVector[selectedSignal].recordBits);      // send Received value/bits
  ELECHOUSE_cc1101.SetRx();      // set Receive on
  mySwitch.disableTransmit();   // set Transmit off
  mySwitch.enableReceive(CC1101riceivePin);   // Receiver on
      
  printConsoleLocal(F("DONE"), TFT_MAGENTA, 1, 0);
  printConsoleLocal("", TFT_MAGENTA, 1, 0);

  myESPboy.myLED.setRGB(0,0,0);
  
  while (!myESPboy.getKeys())delay(10);
  while (myESPboy.getKeys())delay(10);
  }




void show_f(uint16_t selectedSignal) {
  String toPrint;

  selectedSignal--;
  toggleDisplayModeLocal(1);
  
  toPrint = F("Signal: ");
  toPrint+=recordStoredVector[selectedSignal].recordName;
  printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint = F("Value: ");
  toPrint += (String)recordStoredVector[selectedSignal].recordValue;
  toPrint += (" (");
  toPrint += (String)recordStoredVector[selectedSignal].recordBits;
  toPrint += (" Bit)");
  printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

  toPrint = F("Repeat No: ");
  toPrint += (String)recordStoredVector[selectedSignal].recordRepeatno;
  printConsoleLocal(toPrint, TFT_YELLOW, 1, 0);

  toPrint = F("Pulse length: ");
  toPrint += (String)recordStoredVector[selectedSignal].recordPulselen;
  printConsoleLocal(toPrint, TFT_YELLOW, 1, 0);

  toPrint = F("Protocol: ");
  toPrint += (String)recordStoredVector[selectedSignal].recordProtocol;
  printConsoleLocal(toPrint, TFT_YELLOW, 1, 0);
  printConsoleLocal(protDecode[recordStoredVector[selectedSignal].recordProtocol], TFT_WHITE, 1, 0);
  
  printConsoleLocal(F("DONE"), TFT_MAGENTA, 1, 0);
  while (!myESPboy.getKeys())delay(10);
  while (myESPboy.getKeys())delay(10);
  printConsoleLocal("", TFT_MAGENTA, 1, 0);
}




void listofstored_f(){
 uint16_t selectedSignal;
 uint8_t exitFlag;
 uint8_t userInput;
 String userInputName;
 String toPrint;

gotolabel:

  selectedSignal=0;
  exitFlag=0;
  userInput=0;
  userInputName="";

  for (uint16_t i=0; i<recordStoredVector.size(); i++){
    menuList[i]=recordStoredVector[i].recordName;}
  menuList[recordStoredVector.size()] = NULL;

  selectedSignal = (menuInitLocal((const char **)menuList, TFT_YELLOW, TFT_BLUE, TFT_BLUE));
  
  if(selectedSignal){
    while(!exitFlag)
      switch (menuInitLocal(menuRecord, TFT_YELLOW, TFT_BLUE, TFT_BLUE)){
        case send_:
          send_f(selectedSignal);
          break;
        case show_:
          show_f(selectedSignal);
          break;
        case setrepeat_:
          printConsoleLocal(F("Enter signal send repeat No"), TFT_MAGENTA, 1, 0);
          toPrint = F("for ");
          toPrint+=recordStoredVector[selectedSignal-1].recordName;
          printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
          while (!userInput) userInput = getUserInputLocal().toInt();
          if (userInput > DEFAULT_SIGNAL_REPEAT_NUMBER*10) userInput = DEFAULT_SIGNAL_REPEAT_NUMBER;
          recordStoredVector[selectedSignal-1].recordRepeatno = userInput;
          printInfoMessage(F("DONE"));
          userInput=0;
          break;
        case rename_:
          printConsoleLocal(F("Enter new name"), TFT_MAGENTA, 1, 0);
          toPrint = F("for ");
          toPrint+=recordStoredVector[selectedSignal-1].recordName;
          printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
          while (!userInputName.length()) userInputName = getUserInputLocal();
          userInputName = userInputName.substring(0,18);
          memcpy(&recordStoredVector[selectedSignal-1].recordName, userInputName.c_str(), userInputName.length()+1);
          printInfoMessage(F("DONE"));
          userInputName="";
          break;
        case delete_:
          recordStoredVector.erase(recordStoredVector.begin()+(selectedSignal-1));
          while(myESPboy.getKeys()) delay(100);
          if(!recordStoredVector.empty())goto gotolabel;
          exitFlag=1;
          break;
        case cancel_:
          while(myESPboy.getKeys()) delay(100);
          goto gotolabel;
          break;
        default:
        while(myESPboy.getKeys()) delay(100);
        goto gotolabel;
        break;
    }
  }
  else while(myESPboy.getKeys()) delay(100);
}


void printInfoMessage(String messageToPrint){
  toggleDisplayModeLocal(1);
  printConsoleLocal(messageToPrint, TFT_MAGENTA, 1, 0);
  while (!myESPboy.getKeys())delay(10);
  while (myESPboy.getKeys())delay(10);
  printConsoleLocal("", TFT_MAGENTA, 1, 0);
}


void loop(){
  switch (menuInitLocal(menuMain, TFT_YELLOW, TFT_BLUE, TFT_BLUE)){
    case listen_:
      listen_f(0);
      break;
    case listenstore_:
      listen_f(1);
      break;
    case listofstored_:
      if(!recordStoredVector.empty())
        listofstored_f();
      else{
        printInfoMessage(F("Records not found"));}
      break;
    case saveall_:
      toggleDisplayModeLocal(1);
      printConsoleLocal(F("Save to EEPROM..."), TFT_MAGENTA, 1, 0);
      writeEEPROMall();
      printInfoMessage(F("DONE"));
      break;
    case clearall_:
      toggleDisplayModeLocal(1);
      printConsoleLocal(F("Clearing memory..."), TFT_MAGENTA, 1, 0);
      recordStoredVector.clear();
      printInfoMessage(F("DONE"));
      break;
    default:
      delay(150);
      break;
   }
}
