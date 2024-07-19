/*
 * This sketch sends (bogus) thermo / hygro data to a remote weather sensors made by Cresta.
 * 
 * Setup:
 *  - connect transmitter input of a 433MHz transmitter to digital pin 11
 *  - On the weather station, activate the "scan" function for channel 1.
 *
 * Library:
 * https://github.com/mattwire/arduino-dev/tree/master/libraries/RemoteSensor
 * https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
 *
 */
 #include <ELECHOUSE_CC1101_SRC_DRV.h>
 #include <SensorTransmitter.h>

 int pin;
 
 void setup() {

#ifdef ESP32
pin = 2;  // for esp32! Transmit on GPIO pin 2.
#elif ESP8266
pin = 5;  // for esp8266! Transmit on pin 5 = D1
#else
pin = 6;  // for Arduino! Transmit on pin 6.
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
  ELECHOUSE_cc1101.SetTx();       // Transmitt on
 }
 
 void loop() {
   // Initializes a ThermoHygroTransmitter on pin 11, with "random" ID 0, on channel 1.
   ThermoHygroTransmitter transmitter(pin, 0, 1);
   
   // Displays temperatures from -10 degrees Celsius to +20,
   // and humidity from 10% REL to 40% REL, with increments of 2
   for (int i = -10; i<=20; i+=2) {
     // Temperatures are passed at 10 times the real value,
     // to avoid using floating point math.
     transmitter.sendTempHumi(i * 10, i + 20);
     
     // Wait two seconds before sending next.
     delay(2000);
   }
 }
