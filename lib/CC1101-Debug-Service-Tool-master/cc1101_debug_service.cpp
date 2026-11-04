#include "cc1101_debug_service.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>


static const char *Register[] = {

"IOCFG2",
"IOCFG1",
"IOCFG0",
"FIFOTHR",
"SYNC1",
"SYNC0",
"PKTLEN",
"PKTCTRL1",
"PKTCTRL0",
"ADDR",
"CHANNR",
"FSCTRL1",
"FSCTRL0",
"FREQ2",
"FREQ1",
"FREQ0",
"MDMCFG4",
"MDMCFG3",
"MDMCFG2",
"MDMCFG1",
"MDMCFG0",
"DEVIATN",
"MCSM2",
"MCSM1",
"MCSM0",
"FOCCFG",
"BSCFG",
"AGCCTRL2",
"AGCCTRL1",
"AGCCTRL0",
"WOREVT1",
"WOREVT0",
"WORCTRL",
"FREND1",
"FREND0",
"FSCAL3",
"FSCAL2",
"FSCAL1",
"FSCAL0",
"RCCTRL1",
"RCCTRL0",
"FSTEST",
"PTEST",
"AGCTEST",
"TEST2",
"TEST1",
"TEST0",
};

bool startup = 0;
byte m4rxbw;
byte m4dara;
byte m1fec;
byte m1pre;
byte m1chsp;
int slot[6];
//----------------------------------------------------
void CC1101_debug::debug(void){
if (startup == 0){
print_line(50);
//Serial.println("CC1101 service/debug tool v1.0.1 pre-release");
print_line(50);
show_version();
show_register();
startup = 1;
}

if (Serial.available() > 0){
String s = (Serial.readString());
char buf[4];
int val;

const char filter = '=';
int Low = s.indexOf(filter);
String s1 = s.substring(0, Low);
int High = s.indexOf(filter, Low+1);
String s2 = s.substring(Low+1, High);

s2.replace("\n", "");
s2.toCharArray(buf,4);
val = strtol(buf, 0, 16);

for (int i = 0; i<=46; i++){
String r = Register[i];
if (s1 == r+"?\n" && s2 == r+"?"){
print_line(30);
Serial.println("Read Register");
print_line(30);
print_register(i);
i = 47;
}
else if (s1 == r && val>= 0 && val<= 255){
print_line(30);
Serial.println("Write Register");
print_line(30);
ELECHOUSE_cc1101.SpiWriteReg(i,val);
print_register(i);
}
}
if (s == "reg\n"){
show_register();
}
else if (s == "ver\n"){
show_version();
}
else if (s1 == "mhz" && s2 != "mhz"){
write_mhz(s2.toFloat());
}
else if (s == "mhz?\n"){
mhzQ();
}
else if (s1 == "chsp" && s2 != "chsp"){
write_chsp(s2.toFloat());
}
else if (s == "chsp?\n"){
chspQ();
}
else if (s1 == "rxbw" && s2 != "rxbw"){
write_rxbw(s2.toFloat());
}
else if (s == "rxbw?\n"){
rxbwQ();
}
else if (s1 == "drate" && s2 != "drate"){
write_drate(s2.toFloat());
}
else if (s == "drate?\n"){
drateQ();
}
else if (s1 == "chan" && s2 != "chan"){
write_chan(s2.toInt());
}
else if (s == "chan?\n"){
chanQ();
}
else if (s1 == "dev" && s2 != "dev"){
write_deviation(s2.toFloat());
}
else if (s == "dev?\n"){
deviationQ();
}
else if (s == "save?\n"){
show_slot();
}
else if (s1 == "save" && s2.toInt() < 6){
save_slot(s2.toInt());
}
else if (s1 == "load" && s2.toInt() < 6){
load_slot(s2.toInt());
}
else if (s == "rx\n"){
print_line(30);
Serial.println("cc1101 RX");
ELECHOUSE_cc1101.SetRx();
}
else if (s == "tx\n"){
print_line(30);
Serial.println("cc1101 TX");
ELECHOUSE_cc1101.SetTx();
}
else if (s1 == "pa"){
val = s2.toInt(); 
print_line(30);
Serial.print("cc1101 PA ");
Serial.println(val);
ELECHOUSE_cc1101.setPA(val);
}
}
}
//----------------------------------------------------
void CC1101_debug::show_version(void){
print_line(30);
Serial.println("CC1101 Version");
print_line(30);
print_string("PARTNUM");
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(0x30), HEX);
print_string("VERSION");
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(0x31), HEX);
print_string("MARCSTATE");
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(0x35), HEX);
}
//----------------------------------------------------
void CC1101_debug::show_register(void){//debug
print_line(30);
Serial.println("CC1101 Register");
print_line(30);
for (int i = 0; i<=46; i++){
print_register(i);
}
}
//----------------------------------------------------
void CC1101_debug::print_register(byte b){
Serial.print(Register[b]);
int len = strlen(Register[b]);
len = 10 - len;
for (int i = 0; i<len; i++){
Serial.print(" ");
}
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(b), HEX);  
}
//----------------------------------------------------
void CC1101_debug::print_line(byte l){
for (int i = 0; i<l; i++){
Serial.print("-");
}
Serial.println();
}
//----------------------------------------------------
void CC1101_debug::print_string(String s){
Serial.print(s);
int len = s.length();
len = 10 - len;
for (int i = 0; i<len; i++){
Serial.print(" ");
}
}
/*----------------------------------------------------
Frequency functions
----------------------------------------------------*/
void CC1101_debug::mhzQ(void){
print_line(30);
Serial.println("Read MHZ");
print_line(30);
get_mhz();
}
//----------------------------------------------------
void CC1101_debug::write_mhz(float f){
print_line(30);
Serial.println("Write MHZ");
print_line(30);
ELECHOUSE_cc1101.setMHZ(f);
get_mhz();
}
//----------------------------------------------------
void CC1101_debug::get_mhz(void){
float f;
f = ELECHOUSE_cc1101.SpiReadStatus(13)*26;
f = ELECHOUSE_cc1101.SpiReadStatus(14)*0.1015625 + f;
f = ELECHOUSE_cc1101.SpiReadStatus(15)*0.00039675 + f;
print_string("Base Freq");
Serial.print(f,3);
Serial.println(" Mhz");
float c = 25.390625;
for (int i = 0; i<ELECHOUSE_cc1101.SpiReadStatus(20); i++){c+=0.099182;}
Split_MDMCFG1();
for (int i = 0; i<m1chsp; i++){c*=2;}
c*=ELECHOUSE_cc1101.SpiReadStatus(10);
c/=1000;
c+=f;
print_string("Car. Freq");
Serial.print(c,3);
Serial.println(" Mhz");
print_string("Channel");
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(10));
print_register(13);
print_register(14);
print_register(15);
}

/*----------------------------------------------------
Channel functions
----------------------------------------------------*/
void CC1101_debug::chanQ(void){
print_line(30);
Serial.println("Read Chan");
print_line(30);
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(10));
}
//----------------------------------------------------
void CC1101_debug::write_chan(int c){
print_line(30);
Serial.println("Write Chan");
print_line(30);
ELECHOUSE_cc1101.SpiWriteReg(10,c);
Serial.println(ELECHOUSE_cc1101.SpiReadStatus(10));
}
/*----------------------------------------------------
Channel spacing functions
----------------------------------------------------*/
void CC1101_debug::chspQ(void){
print_line(30);
Serial.println("Read CHSP");
print_line(30);
show_chsp();
}
//----------------------------------------------------
void CC1101_debug::show_chsp(void){
float f = 25.390625;
for (int i = 0; i<ELECHOUSE_cc1101.SpiReadStatus(20); i++){f+=0.099182;}
Split_MDMCFG1();
for (int i = 0; i<m1chsp; i++){f*=2;}
print_string("CHSP");
Serial.print(f,6);
Serial.println(" kHz");
print_register(19);
print_register(20);
}
//----------------------------------------------------
void CC1101_debug::write_chsp(float f){
print_line(30);
Serial.println("Write CHSP");
print_line(30);
//get_chsp(f);
ELECHOUSE_cc1101.setChsp(f);
show_chsp();
}
//----------------------------------------------------
void CC1101_debug::Split_MDMCFG1(void){
int calc = ELECHOUSE_cc1101.SpiReadStatus(19);
int s1 = 0;
int s2 = 0;
for (bool i = 0; i==0;){
if (calc >= 128){calc -= 128; s1++;}
else if (calc >= 16){calc -= 16; s2++;}
else{i=1;}
}
m1fec = s1 * 128;
m1pre = s2 * 16;
m1chsp = calc;
}
/*----------------------------------------------------
Receive bandwidth functions
----------------------------------------------------*/
void CC1101_debug::rxbwQ(void){
print_line(30);
Serial.println("Read RXBW");
print_line(30);
show_rxbw();
}
//----------------------------------------------------
void CC1101_debug::show_rxbw(void){
int calc = ELECHOUSE_cc1101.SpiReadStatus(16);
float f = 101.5625;
int s1 = 0;
int s2 = 0;
for (bool i = 0; i==0;){
if (calc >= 64){calc -= 64; s1++;}
else if (calc >= 16){calc -= 16; s2++;}
else{i=1;}
}
float s = 1;
for (int i = 0; i<s2; i++){
s += 0.25;
}
f = f/s;
for (int i = s1; i<3; i++){
f*=2;
}
print_string("RXBW");
Serial.print(f,6);
Serial.println(" kHz");
print_register(16);
}
//----------------------------------------------------
void CC1101_debug::write_rxbw(float f){
print_line(30);
Serial.println("Write RXBW");
print_line(30);
//get_rxbw(f);
ELECHOUSE_cc1101.setRxBW(f);
show_rxbw();
}
//----------------------------------------------------
void CC1101_debug::Split_MDMCFG4(void){
int calc = ELECHOUSE_cc1101.SpiReadStatus(16);
int s1 = 0;
int s2 = 0;
for (bool i = 0; i==0;){
if (calc >= 64){calc -= 64; s1++;}
else if (calc >= 16){calc -= 16; s2++;}
else{i=1;}
}
s1 *= 64;
s2 *= 16;
s1 += s2;
m4rxbw = s1;
s1 = ELECHOUSE_cc1101.SpiReadStatus(16)-s1;
m4dara = s1;
}
/*----------------------------------------------------
Data rate functions
----------------------------------------------------*/
void CC1101_debug::drateQ(void){
print_line(30);
Serial.println("Read DataRate");
print_line(30);
show_drate();
}
//----------------------------------------------------
void CC1101_debug::write_drate(float d){
print_line(30);
Serial.println("Write DataRate");
print_line(30);
//get_drate(d);
ELECHOUSE_cc1101.setDRate(d);
show_drate();
}
//----------------------------------------------------
void CC1101_debug::show_drate(void){
int calc = ELECHOUSE_cc1101.SpiReadStatus(17);
float f = 0.0247955;
for (int i = 0; i<calc; i++){f+=0.000096858;}
Split_MDMCFG4();
for (int i = 0; i<m4dara; i++){f*=2;}
print_string("DRATE");
Serial.print(f,7);
Serial.println(" kBaud");
print_register(16);
print_register(17);
}
/*----------------------------------------------------
Deviation functions
----------------------------------------------------*/
void CC1101_debug::deviationQ(void){
print_line(30);
Serial.println("Read Deviation");
print_line(30);
show_deviation();
}
//----------------------------------------------------
void CC1101_debug::write_deviation(float d){
print_line(30);
Serial.println("Write Deviation");
print_line(30);
//get_deviation(d);
ELECHOUSE_cc1101.setDeviation(d);
show_deviation();  
}
//----------------------------------------------------
void CC1101_debug::show_deviation(void){
int calc = ELECHOUSE_cc1101.SpiReadStatus(21);
float f = 1.586914;
float v = 0.19836425;
int c = 0;
for (int i = 0; i<calc; i++){
f+=v;
if (c==7){v*=2;c=-1;i+=8;}
c++;
}
print_string("Deviation");
Serial.print(f,7);
Serial.println(" kHz");
print_register(21);
}
//----------------------------------------------------
/*void CC1101_debug::get_deviation(float d){
float f = 1.586914;
float v = 0.19836425;
int c = 0;
if (d > 380.859375){d = 380.859375;}
if (d < 1.586914){d = 1.586914;}
for (int i = 0; i<255; i++){
f+=v;
if (c==7){v*=2;c=-1;i+=8;}
if (f>=d){c=i;i=255;}
c++;
}
ELECHOUSE_cc1101.SpiWriteReg(21,c);
}*/
/*----------------------------------------------------
EEPROM functions
----------------------------------------------------*/
void CC1101_debug::save_slot(int s){
print_line(30);
Serial.println("Save Slot");
print_line(30);
int sp = 50*(s+1);
int st = sp-50;
#ifdef ESP32 || ESP8266
EEPROM.begin(512);
#endif
EEPROM.write(st,ELECHOUSE_cc1101.SpiReadStatus(0x31));
for (int i = 1+st; i<sp-2; i++){
EEPROM.write(i,ELECHOUSE_cc1101.SpiReadStatus(i-1-st));
}
#ifdef ESP32 || ESP8266
EEPROM.commit();
#endif
Serial.print("Slot");
Serial.print(s);
Serial.println(" saved");
}
//----------------------------------------------------
void CC1101_debug::load_slot(int s){
print_line(30);
Serial.println("Load Slot");
print_line(30);
int sp = 50*(s+1);
int st = sp-50;
get_slot(s);
if (slot[s] == 1){
#ifdef ESP32 || ESP8266
EEPROM.begin(512);
#endif
for (int i = 1+st; i<sp-2; i++){
ELECHOUSE_cc1101.SpiWriteReg(i-1-st,EEPROM.read(i));
}
Serial.print("Slot");
Serial.print(s);
Serial.println(" loaded");
}
if (slot[s] == 2){Serial.println("Slot empty");}
if (slot[s] == 3){Serial.println("other data");}
}
//----------------------------------------------------
void CC1101_debug::show_slot(void){
print_line(30);
Serial.println("Read Slots");
print_line(30);
for (int c = 0; c<6; c++){
get_slot(c);
Serial.print("Slot");
Serial.print(c);
if (slot[c] == 1){Serial.println(" found Save");}
if (slot[c] == 2){Serial.println(" empty");}
if (slot[c] == 3){Serial.println(" other data");}
}
}
//----------------------------------------------------
void CC1101_debug::get_slot(int s){
#ifdef ESP32 || ESP8266
EEPROM.begin(512);
#endif
byte val1;
byte val2;
int stop = 50*(s+1);
int start = stop-50;
for (int i = start; i<stop; i++){
val1 = EEPROM.read(i);
if (i == start && val1 == ELECHOUSE_cc1101.SpiReadStatus(0x31)){slot[s] = 1; i=stop;}
else if (i>start && slot[s] != 3 && val1==val2){slot[s]=2;}
else if (i>start){slot[s]=3;}
val2 = val1;
}
}

CC1101_debug cc1101_debug;
