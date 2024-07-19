/*
 Basic ESPiLight receive raw (pulse train) signal example

 https://github.com/puuu/espilight
*/
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <ESPiLight.h>

int RECEIVER_PIN;  // any intterupt able pin
int TRANSMITTER_PIN;

ESPiLight rf(TRANSMITTER_PIN);  // use -1 to disable transmitter

// callback function. It is called on successfully received and parsed rc signal
void rfRawCallback(const uint16_t* codes, size_t length) {
  // print pulse lengths
  Serial.print("RAW signal: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print(codes[i]);
    Serial.print(' ');
  }
  Serial.println();

  // format of pilight USB Nano
  String data = rf.pulseTrainToString(codes, length);
  Serial.print("string format: ");
  Serial.print(data);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

#ifdef ESP32
RECEIVER_PIN = 4; TRANSMITTER_PIN = 2;  // for esp32! Receiver on GPIO pin 4. Transmit on GPIO pin 2.
#elif ESP8266
RECEIVER_PIN = 4; TRANSMITTER_PIN = 5;  // for esp8266! Receiver on pin 4 = D2. Transmit on pin 5 = D1.
#else
RECEIVER_PIN = 0; TRANSMITTER_PIN = 6;  // for Arduino! Receiver on interrupt 0 => that is pin #2. Transmit on pin 6.
#endif

  if (ELECHOUSE_cc1101.getCC1101()){    // Check the CC1101 Spi connection.
  Serial.println("Connection OK");
  }else{
  Serial.println("Connection Error");
  }

//CC1101 Settings:                (Settings with "//" are optional!)
  ELECHOUSE_cc1101.Init();            // must be set to initialize the cc1101!
//ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
//ELECHOUSE_cc1101.setPA(10);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
  ELECHOUSE_cc1101.setMHZ(433.92); // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.SetRx();       // set Receive on
  
  // set callback funktion for raw messages
  rf.setPulseTrainCallBack(rfRawCallback);
  // inittilize receiver
  rf.initReceiver(RECEIVER_PIN);
}

void loop() {
  // process input queue and may fire calllback
  rf.loop();
  delay(10);
}
