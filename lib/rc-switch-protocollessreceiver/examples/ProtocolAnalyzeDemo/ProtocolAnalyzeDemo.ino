/*
  Example for analyzing and proposing unknown new protocols
  
  https://github.com/Martin-Laclaustra/rc-switch/

  Requires modified rc-switch branch "protocollessreceiver"
  with ReceivedInverted() function exposed.
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
                              // Use mySwitch.enableReceive(D2); for ESP8266
}

void loop() {
  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      
      int databuffer[64]; // get a copy of the received timings before they are overwritten
      int numberoftimings = 2 * mySwitch.getReceivedBitlength() + 2;
      if(numberoftimings > 64) numberoftimings = 64;
      for (int i = 0; i < numberoftimings; i++) {
        databuffer[i] = mySwitch.getReceivedRawdata()[i];
      }

      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
      
      unsigned int databitsoffset = abs( (int)mySwitch.getReceivedLevelInFirstTiming() - (int)mySwitch.getReceivedInverted());
      //Serial.println( mySwitch.getReceivedLevelInFirstTiming() );
      //Serial.println( mySwitch.getReceivedInverted() );
      //Serial.println( databitsoffset );
      unsigned long dataduration = 0;
      for (unsigned int i = 1 + databitsoffset; i < numberoftimings - 1 + databitsoffset; i++) {
        dataduration += databuffer[i];
      }
      Serial.print("data bits of pulse train duration: ");
      Serial.println( dataduration );
      unsigned int averagebitduration = (int)(0.5 + ((double)dataduration)/mySwitch.getReceivedBitlength());
      unsigned int protocolratio = (unsigned int)(0.5 + ((double)(averagebitduration - mySwitch.getReceivedDelay())) / (double)mySwitch.getReceivedDelay());
      Serial.print("proposed protocol: { ");
      Serial.print(mySwitch.getReceivedDelay());
      Serial.print(", { ");
      Serial.print( (databitsoffset==0) ? 
        (int) (0.5 + (double)databuffer[2*mySwitch.getReceivedBitlength()+1]/(double)mySwitch.getReceivedDelay())
        :
        (int) (0.5 + (double)databuffer[0]/(double)mySwitch.getReceivedDelay())
        );
      Serial.print(", ");
      Serial.print( (databitsoffset==0) ?
        (int) (0.5 + (double)databuffer[0]/(double)mySwitch.getReceivedDelay())
        :
        (int) (0.5 + (double)databuffer[1]/(double)mySwitch.getReceivedDelay())
        );
      Serial.print(" }, { ");
      Serial.print("1");
      Serial.print(", ");
      Serial.print(protocolratio);
      Serial.print(" }, { ");
      Serial.print(protocolratio);
      Serial.print(", ");
      Serial.print("1");
      Serial.print(" }, ");
      Serial.print((mySwitch.getReceivedInverted()) ? "true" : "false" );
      Serial.println(" }");

      // raw signal
      Serial.println("====");
      Serial.print("first level ");
      Serial.println((mySwitch.getReceivedLevelInFirstTiming() == 0) ? "down" : "up" );
      for (int i = 0; i < 2*mySwitch.getReceivedBitlength()+2 - 1 + databitsoffset; i++) {
        Serial.print(databuffer[i]);
        Serial.print(" ");
        if((i - databitsoffset) % 16 == 0) Serial.println("");
      }
      if ((2*mySwitch.getReceivedBitlength()+2 - 1 + databitsoffset - 1) % 16 != 0) Serial.println("");
      if (databitsoffset != 1) Serial.println(databuffer[2*mySwitch.getReceivedBitlength()+1]);
      // plot signal in spreadsheet
      Serial.println("====");
     
    }

    mySwitch.resetAvailable();
  }
}