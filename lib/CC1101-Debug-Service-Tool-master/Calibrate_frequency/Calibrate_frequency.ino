#include <ELECHOUSE_CC1101_SRC_DRV.h>

unsigned long wait;
byte s[3];
int b = 0;
float c;
int gdo0;

void setup() {
#ifdef ESP32
gdo0 = 2;  // for esp32! GDO0 on GPIO pin 2.
#elif ESP8266
gdo0 = 5;  // for esp8266! GDO0 on pin 5 = D1.
#else
gdo0 = 6;  // for Arduino! GDO0 on pin 6.
#endif 
  
Serial.begin(115200);
ELECHOUSE_cc1101.Init();
ELECHOUSE_cc1101.setGDO(gdo0,0);
}

void loop() {
print_line(30);
Serial.println("Calibrate frequency tool");
print_line(30);
Serial.println("Enter for the 315 MHz band. 1");
Serial.println("Enter for the 433 MHz band. 2");
Serial.println("Enter for the 868 MHz band. 3");
Serial.println("Enter for the 915 MHz band. 4");

for (bool i=0; i==0;){
if (Serial.available() > 0){
String f = (Serial.readString());
b=f.toInt();

if(b>0 && b<5){
print_line(30);
Serial.println("Look on your SDR!"); 
}
if (b==1){
print_line(30);
ELECHOUSE_cc1101.setMHZ(300);
Serial.println("Enter if frequncy on 300.000MHz");
i=1;
}
else if (b==2){
print_line(30);
ELECHOUSE_cc1101.setMHZ(378);
Serial.println("Enter the actual frequency at 378.000 MHz");
i=1;
}
else if (b==3){
print_line(30);
ELECHOUSE_cc1101.setMHZ(779);
Serial.println("Enter the actual frequency at 779.000 MHz");
i=1;
}
else if (b==4){
print_line(30);
ELECHOUSE_cc1101.setMHZ(900);
Serial.println("Enter the actual frequency at 900,000 MHz");
i=1;
}else if (b!=0){
Serial.println("This is not possible!\nEnter valid number!");
b=0;
}
}
}

s[0]=b;

for (bool i=0; i==0;){
if (b==1){c=300;}
if (b==2){c=378;}
if (b==3){c=779;}
if (b==4){c=900;}
if (millis()-wait < 2000){
transmitt();
}
else if(millis()-wait > 6000){
wait=millis();
}
if (Serial.available() > 0){
String f = (Serial.readString());
c -= f.toFloat();
c *= 1000;
c/=1.59;
c=ELECHOUSE_cc1101.SpiReadStatus(12)+c;
int c1=c;
c-=c1;
c*=10;
if (c>=5){c1+=1;}
s[1]=c1;
if (c1>=0 && c1<=255){
Serial.println(c1);
i=1;
}else{
Serial.println("This is not possible!\nEnter a valid number!");
}
}
}



print_line(30);
Serial.println("Look on your SDR!"); 
if (b==1){
print_line(30);
ELECHOUSE_cc1101.setMHZ(348);
Serial.println("Enter the actual frequency at 348,000 MHz");
}
else if (b==2){
print_line(30);
ELECHOUSE_cc1101.setMHZ(464);
Serial.println("Enter the actual frequency at 464,000 MHz");
}
else if (b==3){
print_line(30);
ELECHOUSE_cc1101.setMHZ(899);
Serial.println("Enter the actual frequency at 899,000 MHz");
}
else if (b==4){
print_line(30);
ELECHOUSE_cc1101.setMHZ(928);
Serial.println("Enter the actual frequency at 928,000 MHz");
}

for (bool i=0; i==0;){
if (b==1){c=348;}
if (b==2){c=464;}
if (b==3){c=899;}
if (b==4){c=928;}

if (millis()-wait < 2000){
transmitt();
}
else if(millis()-wait > 6000){
wait=millis();
}
if (Serial.available() > 0){
String f = (Serial.readString());
c -= f.toFloat();
c *= 1000;
c/=1.59;
c=ELECHOUSE_cc1101.SpiReadStatus(12)+c;
int c1=c;
c-=c1;
c*=10;
if (c>=5){c1+=1;}
s[2]=c1;
if (c1>=0 && c1<=255){
Serial.println(c1);
i=1;
}else{
Serial.println("This is not possible!\nEnter a valid number!");
}
}
}
print_line(30);
Serial.println("Add this line in Setup of your Sketch:");
print_line(30);
Serial.print("---> ELECHOUSE_cc1101.setClb(");
Serial.print(s[0]);
Serial.print(",");
Serial.print(s[1]);
Serial.print(",");
Serial.print(s[2]);
Serial.println("); <---");
Serial.println("\n\n\n");
}

void print_line(byte l){
for (int i = 0; i<l; i++){
Serial.print("-");
}
Serial.println();
}

void transmitt (){
ELECHOUSE_cc1101.SetTx();
for (int i = 0; i<100; i++){
digitalWrite(gdo0,HIGH);
delayMicroseconds(500);
digitalWrite(gdo0,LOW);
delayMicroseconds(100);
}
ELECHOUSE_cc1101.SetRx();
}
