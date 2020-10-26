#include "ESPboyInit.h"
#include "ESPboyTerminalGUI.h"
#include "ESPboyLED.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

#define CC1101riceivePin    3
#define CC1101chipSelectPin D8
#define TFTchipSelectPin    8

RCSwitch mySwitch;
ESPboyInit myESPboy;
ESPboyTerminalGUI* terminalGUIobj = NULL;
ESPboyLED myLED;

uint32_t counter;

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


void output() {
  String toPrint;
  uint32_t *rawData;
  uint32_t dec, lengthval, rssi, lqi;

  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, LOW);
  delay(10);

  dec = mySwitch.getReceivedValue();
  lengthval = mySwitch.getReceivedBitlength();
  rssi = ELECHOUSE_cc1101.getRssi();
  lqi = ELECHOUSE_cc1101.getLqi();

  myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, HIGH);
  delay(10);

  terminalGUIobj->printConsole(" ", TFT_MAGENTA, 1, 1);
  terminalGUIobj->printConsole(" ", TFT_MAGENTA, 1, 0);
  terminalGUIobj->printConsole(F("-------------------------------"), TFT_YELLOW, 1, 0);
  terminalGUIobj->printConsole(F("DETECTED! DECODE:"), TFT_RED, 1, 0);
  terminalGUIobj->printConsole(F("-------------------------------"), TFT_YELLOW, 1, 0);

  //toPrint = "RSSI/LQI: ";
  //toPrint += (String)rssi;
  //toPrint += "/";
  //toPrint += (String)lqi;
  //terminalGUIobj->printConsole(toPrint, TFT_YELLOW, 1, 0);
  
  toPrint="DEC: ";
  toPrint+=(String)dec;
  toPrint+=" (";
  toPrint+= (String)lengthval;
  toPrint+= " Bit)";
  terminalGUIobj->printConsole(toPrint, TFT_GREEN, 1, 0);

  toPrint="BIN: ";
  toPrint+= String (dec,BIN);
  terminalGUIobj->printConsole(toPrint, TFT_GREEN, 1, 0);

  toPrint="TRI: ";
  toPrint+= bin2tri(dec);
  terminalGUIobj->printConsole(toPrint, TFT_WHITE, 1, 0);

  toPrint="PULSE LEN: ";
  toPrint+=(String)mySwitch.getReceivedDelay();
  toPrint+=" ms";
  terminalGUIobj->printConsole(toPrint, TFT_WHITE, 1, 0);
  
  toPrint="FIRST LEV: ";
  if (mySwitch.getReceivedLevelInFirstTiming()) toPrint += "HIGH";
  else toPrint+= "LOW";
  terminalGUIobj->printConsole(toPrint, TFT_WHITE, 1, 0);

  toPrint="PROTOCOL: ";
  toPrint+=(String)mySwitch.getReceivedProtocol();
  terminalGUIobj->printConsole(toPrint, TFT_WHITE, 1, 0);

  toPrint="IS INV: ";
  if (mySwitch.getReceivedInverted()) toPrint+="YES";
  else toPrint+="NO";
  terminalGUIobj->printConsole(toPrint, TFT_WHITE, 1, 0);

  terminalGUIobj->printConsole(F("DATA: "), TFT_YELLOW, 1, 0);
  toPrint="";
  rawData = mySwitch.getReceivedRawdata();
  for (unsigned int i=0; i<= lengthval*2; i++) toPrint+=((String)rawData[i]+",");
  toPrint = toPrint.substring(0,toPrint.length()-1);
  terminalGUIobj->printConsole(toPrint, TFT_BLUE, 1, 0);

  terminalGUIobj->printConsole(F("-------------------------------"), TFT_YELLOW, 1, 0);
  terminalGUIobj->printConsole("", TFT_BLACK, 1, 0);

  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  myESPboy.mcp.digitalWrite(CC1101chipSelectPin, LOW);
  delay(10);
}



void setup(){
  myESPboy.begin("ESPboy Sub1GHz module");
  myLED.begin(&myESPboy.mcp);
  terminalGUIobj = new ESPboyTerminalGUI(&myESPboy.tft, &myESPboy.mcp);

  terminalGUIobj->printConsole(F("Sub1GHz module v1.0"), TFT_MAGENTA, 1, 0);
  terminalGUIobj->printConsole("", TFT_BLACK, 1, 0);

  myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
  digitalWrite(CC1101chipSelectPin, LOW);
  delay(10);
  
  ELECHOUSE_cc1101.Init();            // must be set to initialize the cc1101!
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setMHZ(433.92); //The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ.  
  mySwitch.enableReceive(CC1101riceivePin);  // Receiver on interrupt 0 => that is pin #2
  ELECHOUSE_cc1101.SetRx();  // set Receive on


}



void loop(){
  static String str="";
  static uint8_t ledFlag = 1;
  
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
    
    myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
    digitalWrite(CC1101chipSelectPin, HIGH);
    delay(10);
       terminalGUIobj->printConsole(str, TFT_MAGENTA, 1, 1);
    myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
    digitalWrite(CC1101chipSelectPin, LOW);

    str+=".";
    if (str.length()>20) str="";
    
    delay(10);
    }
    
  if(ledFlag){
    ledFlag=0;
    myLED.setRGB(0,0,0);
    
    myESPboy.mcp.digitalWrite(TFTchipSelectPin, LOW);
    digitalWrite(CC1101chipSelectPin, HIGH);
    delay(10);
      terminalGUIobj->printConsole(F("Listening..."), TFT_MAGENTA, 1, 0);
      terminalGUIobj->printConsole(F(""), TFT_MAGENTA, 1, 0);
    myESPboy.mcp.digitalWrite(TFTchipSelectPin, HIGH);
    digitalWrite(CC1101chipSelectPin, LOW);
    delay(10);
  }

  delay(1);
}
