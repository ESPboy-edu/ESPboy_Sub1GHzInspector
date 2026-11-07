/*
ESPboy Sub1Ghz inspector
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun

v2.4 Mod: Custom Settings Menu, Live RSSI with Stars, EEPROM Save, FSK Params
*/

#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"
#include "lib/ESPboyTerminalGUI.h"
#include "lib/ESPboyTerminalGUI.cpp"
#include "lib/ESPboyMenuGUI.h"
#include "lib/ESPboyMenuGUI.cpp"

#include "lib/SmartRC-CC1101-Driver-Lib-master/ELECHOUSE_CC1101_SRC_DRV.h"
#include "lib/SmartRC-CC1101-Driver-Lib-master/ELECHOUSE_CC1101_SRC_DRV.cpp"
#include "lib/rc-switch_mod/src/RCSwitch.h"
#include "lib/rc-switch_mod/src/RCSwitch.cpp"
#include <ESP_EEPROM.h>

#define CC1101riceivePin    15  // D8 (GPIO15) - безопасный вход (RX)
#define CC1101sendPin       16  // D0 (GPIO16) - выход (TX)
#define CC1101chipSelectPin 10  // GPB2 на расширителе MCP23017
#define TFTchipSelectPin    8   // GPB0 на расширителе MCP23017

#define APP_ID 0xCCCE 
#define MAX_RECORDS_TO_STORE 40
#define DEFAULT_SIGNAL_REPEAT_NUMBER 3
#define EEPROM_OFFSET 10

String protDecode[]={
"Unknown", "Protocol 01 Princeton, PT-2240", "Protocol 02 AT-Motor?",
"Protocol 03", "Protocol 04", "Protocol 05", "Protocol 06 HT6P20B",
"Protocol 07 HS2303-PT", "Protocol 08 Came 12bit, HT12E",
"Protocol 09 Nice_Flo 12bit", "Protocol 10 V2 phoenix", "Protocol 11 Nice_FloR-S 52bit",
"Protocol 12 Keeloq 64/66 falseok", "Protocol 13 test CFM", "Protocol 14 test StarLine",
"Protocol 15", "Protocol 16 Einhell", "Protocol 17 InterTechno PAR-1000",
"Protocol 18 Intertechno ITT-1500", "Protocol 19 Murcury", "Protocol 20 AC114",
"Protocol 21 DC250", "Protocol 22 Mandolyn/Lidl", "Protocol 23 Lidl TR-502MSV",
"Protocol 24 Lidl TR-502MSV/RC701", "Protocol 25 NEC", "Protocol 26 Arlec RC210",
"Protocol 27 Zap, FHT-7901", "Protocol 28", "Protocol 29 NEXA", "Protocol 30 Anima",
"Protocol 31 Mertik Maxitrol G6R-H4T1", "Protocol 32", "Protocol 33 Dooya Control DC2708L",
"Protocol 34 DIGOO SD10 ", "Protocol 35 Dooya 5-Channel", "Protocol 36 DC2700AC",
"Protocol 37 DEWENWILS Power Strip", "Protocol 38 Nexus weather, 36 bit", "Protocol 39 Louvolite"
};

char *menuList[MAX_RECORDS_TO_STORE+1];

const char *menuMain[] PROGMEM = {
"LISTEN", "LISTEN & STORE", "LIST OF STORED", "SAVE ALL", "CLEAR ALL", "SETTINGS", NULL
};

const char *menuRecord[] PROGMEM = {
"SEND", "SHOW", "SET REPEAT", "RENAME", "DELETE", "CANCEL", NULL
};

enum {listen_=1, listenstore_, listofstored_, saveall_, clearall_, settings_};
enum {send_=1, show_, setrepeat_, rename_, delete_, cancel_};

// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ НАСТРОЕК CC1101 ---
float currentFreq = 433.92;
uint8_t currentMod = 2; // 2 = ASK/OOK, 0 = 2-FSK, 1 = GFSK
uint16_t currentRxBw = 600;
float currentDRate = 4.8;
float currentDev = 47.6;
// ---------------------------------------------

RCSwitch mySwitch;
ESPboyInit myESPboy;
ESPboyTerminalGUI *terminalGUIobj = NULL;
ESPboyMenuGUI *menuGUIobj = NULL;

struct EEPROMinitStruct{
  uint32_t appId = APP_ID;
  uint8_t  recordsQuantity = 0;
  float    freq = 433.92;
  uint8_t  mod = 2;
  uint16_t rxbw = 600;
  float    drate = 4.8;
  float    dev = 47.6;
}EEPROMinit;

struct recordStored{
char     recordName[20];
uint32_t recordRepeatno;
uint32_t recordProtocol;
uint32_t recordPulselen;
uint32_t recordBits;
uint32_t recordValue;
};

std::vector<recordStored> recordStoredVector;

String bin2tri(uint32_t dec) {
uint8_t pos = 0;
String returnVal="";
String bin = String(dec,BIN);
while (pos < (bin.length()-1)) {
if (bin[pos]=='0' && bin[pos+1]=='0') returnVal += '0';
else if (bin[pos]=='1' && bin[pos+1]=='1') returnVal += '1';
else if (bin[pos]=='0' && bin[pos+1]=='1') returnVal += 'F';
else return "NO";
pos += 2;
}
return returnVal;
}

// ---------------- Обертки для GUI (переведены на MCP23017, ИСПРАВЛЕНЫ) ----------------
void printConsoleLocal(String str, uint32_t clr, uint8_t flag1, uint8_t flag2){
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH); 
myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);     
terminalGUIobj->printConsole(str, clr, flag1, flag2);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);    
}

String getUserInputLocal(){
String inputLocal;
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
inputLocal = terminalGUIobj->getUserInput();
myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
return (inputLocal);
}

void toggleDisplayModeLocal(uint8_t displayMode){
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
terminalGUIobj->toggleDisplayMode(displayMode);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
}

void doScrollLocal(){
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
terminalGUIobj->doScroll();
myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
}

// Пробрасываем параметр returnOnArrowKeys
uint16_t menuInitLocal(const char** menuLines, uint16_t menuLineColor, uint16_t menuUnselectedLineColor, uint16_t menuSelectionColor, bool returnOnArrowKeys = false){
  uint16_t menuItemSelected;
  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  menuItemSelected = menuGUIobj->menuInit(menuLines, menuLineColor, menuUnselectedLineColor, menuSelectionColor, returnOnArrowKeys);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  return (menuItemSelected);
}
// --------------------------------------------------------------------------

void drawDecodedSignal() {
String toPrint;
uint32_t *rawData;
uint32_t dec, lengthval, rssi, lqi;

int16_t databuffer[64];
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

uint16_t databitsoffset = 0;
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

if(!mySwitch.getReceivedProtocol())printConsoleLocal(toPrint, TFT_RED, 1, 0);
else printConsoleLocal(toPrint, TFT_GREEN, 1, 0);

toPrint="PULSE LEN: ";
toPrint+=(String)mySwitch.getReceivedDelay();
toPrint+=" ms";
printConsoleLocal(toPrint, TFT_WHITE, 1, 0);

toPrint = "RSSI: " + String(rssi) + " dBm   LQI: " + String(lqi);
printConsoleLocal(toPrint, TFT_CYAN, 1, 0);

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
  
  EEPROMinit.freq = 433.92;
  EEPROMinit.mod = 2;
  EEPROMinit.rxbw = 600;
  EEPROMinit.drate = 4.8;
  EEPROMinit.dev = 47.6;
  
  EEPROM.put(EEPROM_OFFSET, EEPROMinit);
  EEPROM.commit();
}

currentFreq = EEPROMinit.freq;
currentMod = EEPROMinit.mod;
currentRxBw = EEPROMinit.rxbw;
currentDRate = EEPROMinit.drate;
currentDev = EEPROMinit.dev;

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

void mcp_cs_write(bool state) {
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, state);
}

void setup(){
Serial.begin(115200);
Serial.println("\nStarting ESPboy Sub1GHz Inspector...");

EEPROM.begin(2048);
readEEPROM();

myESPboy.begin("Sub1GHz inspector v2");

terminalGUIobj = new ESPboyTerminalGUI(&myESPboy.tft, &myESPboy.mcp);
menuGUIobj = new ESPboyMenuGUI(&myESPboy);

myESPboy.mcp.pinMode(CC1101chipSelectPin, OUTPUT);
myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);

ELECHOUSE_cc1101.setCS_MCP(mcp_cs_write);
ELECHOUSE_cc1101.setSpiPin(14, 12, 13, 0);
ELECHOUSE_cc1101.setGDO(CC1101sendPin, CC1101riceivePin);
ELECHOUSE_cc1101.Init();

byte partnum = ELECHOUSE_cc1101.SpiReadStatus(0x30);
byte version = ELECHOUSE_cc1101.SpiReadStatus(0x31);
if(partnum != 0x00 || (version != 0x04 && version != 0x14)){
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
  myESPboy.tft.setTextColor(0xF800);
  myESPboy.tft.drawString("CC1101 SPI ERROR", 20 ,40);
  myESPboy.tft.drawString("Check wiring!", 30 ,55);
  while(1) delay(100);
} 

  ELECHOUSE_cc1101.setMHZ(currentFreq); 
  ELECHOUSE_cc1101.setModulation(currentMod);
  ELECHOUSE_cc1101.setRxBW(currentRxBw);
  ELECHOUSE_cc1101.setDRate(currentDRate);
  ELECHOUSE_cc1101.setDeviation(currentDev);

  ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL2, 0xC7); 
  ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL1, 0x00); 
  ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL0, 0xB2);

mySwitch.enableReceive(digitalPinToInterrupt(CC1101riceivePin));
ELECHOUSE_cc1101.SetRx();
}

void listen_f(uint8_t storeFlag){
  String str="";
  uint8_t ledFlag = 1;
  uint32_t counter = millis();
  bool isScanning = true; 

  mySwitch.resetAvailable();
  toggleDisplayModeLocal(1); 
 
  while(true){
    uint8_t pressedButtons = myESPboy.getKeys();
    
    if(pressedButtons & PAD_ESC){
      break;
    }

    if (isScanning && (pressedButtons & PAD_ACT || pressedButtons & PAD_RGT || pressedButtons & PAD_LFT)) {
      isScanning = false;
      printConsoleLocal(F("Stop"), TFT_MAGENTA, 1, 1);
      printConsoleLocal(F("LGT/RGT-scroll. ACT-resume. B-exit"), TFT_MAGENTA, 1, 0);
      
      if (pressedButtons & PAD_ACT) {
         while(myESPboy.getKeys()) delay(10); 
      }
      continue; 
    }

    if (!isScanning) {
      if (pressedButtons & PAD_RGT || pressedButtons & PAD_LFT) {
        doScrollLocal(); 
        delay(50);       
        continue;
      }
      
      if (pressedButtons & PAD_ACT) {
        isScanning = true;
        ledFlag = 1;
        str = "";
        counter = millis();
        mySwitch.resetAvailable(); 
        while(myESPboy.getKeys()) delay(10); 
        continue;
      }
    }

    if (isScanning) {
      if (mySwitch.available()) {
        myESPboy.myLED.setRGB(0,20,0);
        ledFlag = 1;
        counter = millis();
        str = "";
        myESPboy.playTone(100,100);
        drawDecodedSignal();
        
        if (storeFlag){
          if (!mySwitch.getReceivedProtocol()){
            printConsoleLocal(F("Can't store UNKNOWN PROTOCOL"), TFT_RED, 1, 0);
          } else {
            if(recordStoredVector.size()<MAX_RECORDS_TO_STORE){
              storeSignal();
              String toPrint = F("Signal stored. TOTAL: ");
              toPrint += String (recordStoredVector.size());
              toPrint +=F(" of ");
              toPrint += (String)MAX_RECORDS_TO_STORE;
              printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
            } else {
              String toPrint = F("Can't store. Max reached ");
              toPrint += (String)MAX_RECORDS_TO_STORE;
              printConsoleLocal(toPrint, TFT_MAGENTA, 1, 0);
            }
          }   
        }
        mySwitch.resetAvailable();
      }
    
      // --- МОНИТОР LIVE RSSI (УКОРОЧЕННЫЙ ВЫВОД + ЗВЕЗДОЧКИ) ---
      if(millis()-counter > 500){ 
        myESPboy.myLED.setRGB(0,20,0);
        counter=millis();
        
        int liveRssi = ELECHOUSE_cc1101.getRssi(); 
        
        String stars;
        if (liveRssi < -95)      stars = "*    ";
        else if (liveRssi < -80) stars = "**   ";
        else if (liveRssi < -65) stars = "***  ";
        else if (liveRssi < -50) stars = "**** ";
        else                     stars = "*****";
        
        String liveStr = "RSSI:" + String(liveRssi) + " " + stars + " ";
        printConsoleLocal(liveStr, TFT_MAGENTA, 1, 1); 
        
        myESPboy.myLED.setRGB(0,0,0);
      }
      // ---------------------------------------------------------
      
      if(ledFlag){
        ledFlag=0;
        myESPboy.myLED.setRGB(0,0,0);
        
        // --- ВЫВОД ПАРАМЕТРОВ ПЕРЕД СКАНИРОВАНИЕМ ---
        String s_mod = (currentMod == 2) ? "ASK" : (currentMod == 0) ? "2FSK" : "GFSK";
        String p1 = "F:" + String(currentFreq) + " M:" + s_mod;
        String p2 = "BW:" + String(currentRxBw) + " D:" + String(currentDRate);
        String p3 = "DEV:" + String(currentDev);
        
        printConsoleLocal(p1, TFT_YELLOW, 1, 0);
        printConsoleLocal(p2, TFT_YELLOW, 1, 0);
        printConsoleLocal(p3, TFT_YELLOW, 1, 0);
        
        printConsoleLocal(F("Listening..."), TFT_MAGENTA, 1, 0);
        printConsoleLocal(F(""), TFT_MAGENTA, 1, 0); // Пустая строка для перезаписи RSSI
        // --------------------------------------------
      }
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

ELECHOUSE_cc1101.SetTx();
mySwitch.disableReceive();
mySwitch.enableTransmit(CC1101sendPin);
mySwitch.setRepeatTransmit(recordStoredVector[selectedSignal].recordRepeatno);
mySwitch.setProtocol(recordStoredVector[selectedSignal].recordProtocol);
mySwitch.setPulseLength(recordStoredVector[selectedSignal].recordPulselen);
mySwitch.send(recordStoredVector[selectedSignal].recordValue,recordStoredVector[selectedSignal].recordBits);
ELECHOUSE_cc1101.SetRx();
mySwitch.disableTransmit();
mySwitch.enableReceive(digitalPinToInterrupt(CC1101riceivePin));

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

// --- МЕНЮ НАСТРОЕК ---
void settings_f() {
  uint8_t exitFlag = 0;
  String lines[6];
  const char *menuSettings[7]; 
  
  while (!exitFlag) {
    lines[0] = "FREQ: " + String(currentFreq) + " MHz";
    String s_mod = "MOD: ";
    if (currentMod == 2) s_mod += "ASK/OOK";
    else if (currentMod == 0) s_mod += "2-FSK";
    else s_mod += "GFSK";
    lines[1] = s_mod;
    lines[2] = "RX BW: " + String(currentRxBw) + " kHz";
    lines[3] = "DRATE: " + String(currentDRate) + " kBaud";
    lines[4] = "DEV: " + String(currentDev) + " kHz";
    lines[5] = "BACK";

    for (int i=0; i<6; i++) menuSettings[i] = lines[i].c_str();
    menuSettings[6] = NULL;

    uint16_t result = menuInitLocal(menuSettings, TFT_YELLOW, TFT_BLUE, TFT_BLUE, true);
    
    uint16_t selectedItem = result & 0x3FFF; 
    
    switch (selectedItem) {
      case 1: 
        if (currentFreq == 433.92f) currentFreq = 315.00f;
        else if (currentFreq == 315.00f) currentFreq = 868.35f;
        else currentFreq = 433.92f;
        break;
      case 2: 
        if (currentMod == 2) currentMod = 0;      
        else if (currentMod == 0) currentMod = 1; 
        else currentMod = 2;                      
        break;
      case 3: 
        if (currentRxBw == 600) currentRxBw = 300;
        else if (currentRxBw == 300) currentRxBw = 100;
        else currentRxBw = 600;
        break;
      case 4: 
        if (currentDRate == 4.8f) currentDRate = 9.6f;
        else if (currentDRate == 9.6f) currentDRate = 19.2f;
        else if (currentDRate == 19.2f) currentDRate = 1.2f;
        else currentDRate = 4.8f;
        break;
      case 5: 
        if (currentDev == 47.6f) currentDev = 25.4f;
        else if (currentDev == 25.4f) currentDev = 9.5f;
        else if (currentDev == 9.5f) currentDev = 4.7f;
        else currentDev = 47.6f;
        break;
      case 6: 
      case 0: 
        exitFlag = 1;
        break;
    }
  }

  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  myESPboy.tft.fillScreen(TFT_BLACK);
  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);

  ELECHOUSE_cc1101.SpiStrobe(0x36); 
  ELECHOUSE_cc1101.setMHZ(currentFreq);
  ELECHOUSE_cc1101.setModulation(currentMod);
  ELECHOUSE_cc1101.setRxBW(currentRxBw);
  ELECHOUSE_cc1101.setDRate(currentDRate);
  ELECHOUSE_cc1101.setDeviation(currentDev);
  ELECHOUSE_cc1101.SetRx(); 
  
  EEPROMinit.freq = currentFreq;
  EEPROMinit.mod = currentMod;
  EEPROMinit.rxbw = currentRxBw;
  EEPROMinit.drate = currentDRate;
  EEPROMinit.dev = currentDev;
  EEPROM.put(EEPROM_OFFSET, EEPROMinit);
  EEPROM.commit();
}
// ---------------------------------------------

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
case settings_:
settings_f();
break;
default:
delay(150);
break;
}
}