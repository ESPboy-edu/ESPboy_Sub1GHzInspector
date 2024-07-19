/*
  Simple example for Receiving and Transmit decimal code for cc1101
  
  https://github.com/sui77/rc-switch/
  https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
  ----------------------------------------------------------
  Mod by Little Satan. Have Fun!
  ----------------------------------------------------------
*/
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

int pinRx; // int for Receive pin.
int pinTx; // int for Transmit pin.

RCSwitch mySwitch = RCSwitch();

int buttonPin;                 // for button pin.
int buttonState = 0;          // button state
int ccSetRx = 0;             // reset state for Receive
int long value = 5393;      // int to save value
int bits = 24;             // int to save bit number
int prot = 1;             // int to save Protocol number
int puls = 320;          // int to save pulse length

void setup() {
  Serial.begin(9600);

#ifdef ESP32
pinRx = 4; pinTx = 2;  // for esp32! Receiver on GPIO pin 4. Transmit on GPIO pin 2.
buttonPin = 34; // set button on GPIO pin 34.
#elif ESP8266
pinRx = 4; pinTx = 5;  // for esp8266! Receiver on pin 4 = D2. Transmit on pin 5 = D1.
buttonPin = 16; // set button on pin 16 = D0.
#else
pinRx = 0; pinTx = 6;  // for Arduino! Receiver on interrupt 0 => that is pin #2. Transmit on pin 6.
buttonPin = 4;  // set button on pin D4.
#endif 

  if (ELECHOUSE_cc1101.getCC1101()){       // Check the CC1101 Spi connection.
  Serial.println("Connection OK");
  }else{
  Serial.println("Connection Error");
  }

//CC1101 Settings:                (Settings with "//" are optional!)
  ELECHOUSE_cc1101.Init();            // must be set to initialize the cc1101!
//ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
//ELECHOUSE_cc1101.setPA(10);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
  ELECHOUSE_cc1101.setMHZ(433.92); // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.

  pinMode(buttonPin, INPUT);     // set pin4 as input

}
void loop() {

   buttonState = digitalRead(buttonPin);  // read digital pin and save the state to int


   if (buttonState == HIGH) {         // the button is pressed. Set transmit on.

  ccSetRx = 0;                       // set resetstate to 0 for next reinit to Recive
  ELECHOUSE_cc1101.SetTx();         // set Transmit on
  mySwitch.disableReceive();       // Receiver off
  mySwitch.enableTransmit(pinTx); // Transmit on

  mySwitch.setRepeatTransmit(3);  // transmission repetitions.
  mySwitch.setProtocol(prot);    // send Received Protocol
  mySwitch.setPulseLength(puls);// send Received Delay
  mySwitch.send(value, bits);  // send Received value/bits

    Serial.print("Transmit ");
    Serial.print( value );
    Serial.print(" / ");
    Serial.print( bits );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.print( prot );
    Serial.print(" Delay: ");    
    Serial.println( puls );      
   }

   
  if (buttonState == LOW && ccSetRx == 0){  //the button is not pressed. set cc1101 to Receive.
    
  ELECHOUSE_cc1101.SetRx();         // set Receive on
  mySwitch.disableTransmit();      // set Transmit off
  mySwitch.enableReceive(pinRx);  // Receiver on
  ccSetRx = 1;
   }

  if (buttonState == LOW && ccSetRx == 1) {   //the button is not pressed and set receive is finish. Receive values. 



  if (mySwitch.available()){
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.print( mySwitch.getReceivedProtocol() );
    Serial.print(" Delay: ");    
    Serial.println( mySwitch.getReceivedDelay() );

    value =  mySwitch.getReceivedValue();        // save received Value
    bits = mySwitch.getReceivedBitlength();     // save received Bitlength
    prot = mySwitch.getReceivedProtocol();     // save received Protocol
    puls =  mySwitch.getReceivedDelay();      // save received pulse length
    
    mySwitch.resetAvailable();
   
   }
   }

}
