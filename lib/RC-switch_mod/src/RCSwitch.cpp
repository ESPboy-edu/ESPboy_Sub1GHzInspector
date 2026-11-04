/*
RCSwitch - Arduino libary for remote control outlet switches
Copyright (c) 2011 Suat Özgür.  All right reserved.
*/	
#define KeeLoq_NLF    0x3A5C742E

#include "RCSwitch.h"

#ifdef RaspberryPi
#define PROGMEM
#define memcpy_P(dest, src, num) memcpy((dest), (src), (num))
#endif

#if defined(ESP8266)
#define RECEIVE_ATTR ICACHE_RAM_ATTR
#define VAR_ISR_ATTR
#elif defined(ESP32)
#define RECEIVE_ATTR IRAM_ATTR
#define VAR_ISR_ATTR DRAM_ATTR
#else
#define RECEIVE_ATTR
#define VAR_ISR_ATTR
#endif

#if defined(ESP8266) || defined(ESP32)
static const VAR_ISR_ATTR RCSwitch::Protocol proto[] = {
#else
static const RCSwitch::Protocol PROGMEM proto[] = {
#endif
{ 350,  0, { 0, 0 }, 1, {  1, 31 }, { 1,  3 }, { 3, 1 }, false,  0 },  // 01 (Princeton, PT-2240)
{ 650,  0, { 0, 0 }, 1, {  1, 10 }, { 1,  2 }, { 2, 1 }, false,  0 },  // 02
{ 100,  0, { 0, 0 }, 1, { 30, 71 }, { 4, 11 }, { 9, 6 }, false,  0 },  // 03
{ 380,  0, { 0, 0 }, 1, {  1,  6 }, { 1,  3 }, { 3, 1 }, false,  0 },  // 04
{ 500,  0, { 0, 0 }, 1, {  6, 14 }, { 1,  2 }, { 2, 1 }, false,  0 },  // 05
{ 450,  0, { 0, 0 }, 1, { 23,  1 }, { 1,  2 }, { 2, 1 }, true,   0 },  // 06 (HT6P20B)
{ 150,  0, { 0, 0 }, 1, {  2, 62 }, { 1,  6 }, { 6, 1 }, false,  0 },  // 07 (HS2303-PT)
{ 320,  0, { 0, 0 }, 1, { 36,  1 }, { 1,  2 }, { 2, 1 }, true,   0 },  // 08 (Came 12bit, HT12E)
{ 700,  0, { 0, 0 }, 1, { 32,  1 }, { 1,  2 }, { 2, 1 }, true,   0 },  // 09 (Nice_Flo 12bit)
{ 420,  0, { 0, 0 }, 1, { 60,  6 }, { 1,  2 }, { 2, 1 }, true,   0 },  // 10 (V2 phoenix)
{ 500,  2, { 3, 3 }, 0, {  0,  0 }, { 1,  2 }, { 2, 1 }, false, 37 },  // 11 (Nice_FloR-S 52bit)
{ 400, 23, { 1, 1 }, 1, {  0,  9 }, { 2,  1 }, { 1, 2 }, false, 39 },  // 12 (Keeloq 64/66)
{ 300,  6, { 2, 2 }, 3, {  8,  3 }, { 2,  2 }, { 3, 3 }, false,  0 },  // 13 test (CFM)
{ 250, 12, { 4, 4 }, 0, {  0,  0 }, { 1,  1 }, { 2, 2 }, false,  0 },  // 14 test (StarLine)
{ 500,  0, { 0, 0 }, 0, { 100, 1 }, { 1,  2 }, { 2, 1 }, false, 35 },  // 15
{ 361,  0, { 0, 0 }, 1, {  52,  1 }, { 1,  3 }, { 3, 1 }, true,   0 }, // 16 (Einhell)
{ 500,  0, { 0, 0 }, 1, {   1, 23 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 17 (InterTechno PAR-1000)
{ 180,  0, { 0, 0 }, 1, {   1, 15 }, { 1,  1 }, { 1, 8 }, false,  0 }, // 18 (Intertechno ITT-1500)
{ 350,  0, { 0, 0 }, 1, {   1,  2 }, { 0,  2 }, { 3, 2 }, false,  0 }, // 19 (Murcury)
{ 150,  0, { 0, 0 }, 1, {  34,  3 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 20 (AC114)
{ 360,  0, { 0, 0 }, 1, {  13,  4 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 21 (DC250)
{ 650,  0, { 0, 0 }, 1, {   1, 10 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 22 (Mandolyn)
{ 641,  0, { 0, 0 }, 1, { 115,  1 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 23 (Lidl)
{ 620,  0, { 0, 0 }, 1, {   0, 64 }, { 0,  1 }, { 1, 0 }, false,  0 }, // 24 (Lidl RC701)
{ 560,  0, { 0, 0 }, 1, {  16,  8 }, { 1,  1 }, { 1, 3 }, false,  0 }, // 25 (NEC)
{ 385,  0, { 0, 0 }, 1, {   1, 17 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 26 (Arlec RC210)
{ 188,  0, { 0, 0 }, 1, {   1, 31 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 27 (Zap, FHT-7901)
{ 700,  1, { 0, 1 }, 1, { 116,  0 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 28 (Quigg GT-7000)
{ 220,  0, { 0, 0 }, 1, {   1, 46 }, { 1,  6 }, { 1, 1 }, false,  2 }, // 29 (NEXA)
{ 260,  0, { 0, 0 }, 1, {   1,  8 }, { 1,  4 }, { 4, 1 }, true,   0 }, // 30 (Anima)
{ 400,  0, { 0, 0 }, 1, {   1,  1 }, { 1,  2 }, { 2, 1 }, false, 43 }, // 31 (Mertik Maxitrol)
{ 365,  0, { 0, 0 }, 1, {  18,  1 }, { 3,  1 }, { 1, 3 }, true,   0 }, // 32 (1ByOne Doorbell)
{ 340,  0, { 0, 0 }, 1, {  14,  4 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 33 (Dooya Control DC2708L)
{ 120,  0, { 0, 0 }, 1, {   1, 28 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 34 DIGOO SD10
{ 20,   0, { 0, 0 }, 1, { 239, 78 }, {20, 35 }, {35, 20}, false, 10000},// 35 Dooya 5-Channel
{ 250,  0, { 0, 0 }, 1, {  18,  6 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 36 Dooya remote DC2700AC
{ 200,  0, { 0, 0 }, 0, {   0,  0 }, { 1,  3 }, { 3, 1} , false, 20},  // 37 DEWENWILS Power Strip
{ 500,  0, { 0, 0 }, 1, {   0,  7 }, { 1,  2 }, { 1, 4},  false,  0 }, // 38 (Nexus weather, 36 bit)
{ 20,  16, { 14, 30 }, 1, { 510,  30 }, { 14,  30 }, { 30, 14 }, false,  230 }  // 39 (Louvolite) with premable
};

enum {
numProto = sizeof(proto) / sizeof(proto[0])
};

#if not defined( RCSwitchDisableReceiving )
volatile unsigned long long RCSwitch::nReceivedValue = 0;
volatile unsigned long long RCSwitch::nReceiveProtocolMask;
volatile unsigned int RCSwitch::nReceivedBitlength = 0;
volatile unsigned int RCSwitch::nReceivedDelay = 0;
volatile unsigned int RCSwitch::nReceivedProtocol = 0;
int RCSwitch::nReceiveTolerance = 60;
const unsigned int RCSwitch::nSeparationLimit = RCSWITCH_SEPARATION_LIMIT;

// --- ИНИЦИАЛИЗАЦИЯ ОТЛАДОЧНЫХ ПЕРЕМЕННЫХ ---
volatile unsigned long RCSwitch::isrCount = 0;
volatile bool RCSwitch::unknownPacketReady = false;
volatile unsigned int RCSwitch::unknownChangeCount = 0;
unsigned int RCSwitch::unknownTimings[RCSWITCH_MAX_CHANGES];
// -------------------------------------------

unsigned int RCSwitch::timings[RCSWITCH_MAX_CHANGES];
unsigned int RCSwitch::buftimings[4];
#endif

RCSwitch::RCSwitch() {
this->nTransmitterPin = -1;
this->setRepeatTransmit(5);
this->setProtocol(1);
#if not defined( RCSwitchDisableReceiving )
this->nReceiverInterrupt = -1;
this->setReceiveTolerance(60);
RCSwitch::nReceivedValue = 0;
RCSwitch::nReceiveProtocolMask = (1ULL << numProto)-1;
#endif
}

uint8_t RCSwitch::getNumProtos() {
return numProto;
}
void RCSwitch::setProtocol(Protocol protocol) {
this->protocol = protocol;
}
void RCSwitch::setProtocol(int nProtocol) {
if (nProtocol < 1 || nProtocol > numProto) nProtocol = 1;
#if defined(ESP8266) || defined(ESP32)
this->protocol = proto[nProtocol-1];
#else
memcpy_P(&this->protocol, &proto[nProtocol-1], sizeof(Protocol));
#endif
}
void RCSwitch::setProtocol(int nProtocol, int nPulseLength) {
setProtocol(nProtocol);
this->setPulseLength(nPulseLength);
}
void RCSwitch::setPulseLength(int nPulseLength) {
this->protocol.pulseLength = nPulseLength;
}
void RCSwitch::setRepeatTransmit(int nRepeatTransmit) {
this->nRepeatTransmit = nRepeatTransmit;
}
#if not defined( RCSwitchDisableReceiving )
void RCSwitch::setReceiveTolerance(int nPercent) {
RCSwitch::nReceiveTolerance = nPercent;
}
void RCSwitch::setReceiveProtocolMask(unsigned long long mask) {
RCSwitch::nReceiveProtocolMask = mask;
}
#endif
void RCSwitch::enableTransmit(int nTransmitterPin) {
this->nTransmitterPin = nTransmitterPin;
pinMode(this->nTransmitterPin, OUTPUT);
}
void RCSwitch::disableTransmit() {
this->nTransmitterPin = -1;
}
void RCSwitch::switchOn(char sGroup, int nDevice) {
this->sendTriState( this->getCodeWordD(sGroup, nDevice, true) );
}
void RCSwitch::switchOff(char sGroup, int nDevice) {
this->sendTriState( this->getCodeWordD(sGroup, nDevice, false) );
}
void RCSwitch::switchOn(char sFamily, int nGroup, int nDevice) {
this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, true) );
}
void RCSwitch::switchOff(char sFamily, int nGroup, int nDevice) {
this->sendTriState( this->getCodeWordC(sFamily, nGroup, nDevice, false) );
}
void RCSwitch::switchOn(int nAddressCode, int nChannelCode) {
this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, true) );
}
void RCSwitch::switchOff(int nAddressCode, int nChannelCode) {
this->sendTriState( this->getCodeWordB(nAddressCode, nChannelCode, false) );
}
void RCSwitch::switchOn(const char* sGroup, int nChannel) {
const char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
this->switchOn(sGroup, code[nChannel]);
}
void RCSwitch::switchOff(const char* sGroup, int nChannel) {
const char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
this->switchOff(sGroup, code[nChannel]);
}
void RCSwitch::switchOn(const char* sGroup, const char* sDevice) {
this->sendTriState( this->getCodeWordA(sGroup, sDevice, true) );
}
void RCSwitch::switchOff(const char* sGroup, const char* sDevice) {
this->sendTriState( this->getCodeWordA(sGroup, sDevice, false) );
}
char* RCSwitch::getCodeWordA(const char* sGroup, const char* sDevice, bool bStatus) {
static char sReturn[13];
int nReturnPos = 0;
for (int i = 0; i < 5; i++) {
sReturn[nReturnPos++] = (sGroup[i] == '0') ? 'F' : '0';
}
for (int i = 0; i < 5; i++) {
sReturn[nReturnPos++] = (sDevice[i] == '0') ? 'F' : '0';
}
sReturn[nReturnPos++] = bStatus ? '0' : 'F';
sReturn[nReturnPos++] = bStatus ? 'F' : '0';
sReturn[nReturnPos] = '\0';
return sReturn;
}
char* RCSwitch::getCodeWordB(int nAddressCode, int nChannelCode, bool bStatus) {
static char sReturn[13];
int nReturnPos = 0;
if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4) {
return 0;
}
for (int i = 1; i <= 4; i++) {
sReturn[nReturnPos++] = (nAddressCode == i) ? '0' : 'F';
}
for (int i = 1; i <= 4; i++) {
sReturn[nReturnPos++] = (nChannelCode == i) ? '0' : 'F';
}
sReturn[nReturnPos++] = 'F';
sReturn[nReturnPos++] = 'F';
sReturn[nReturnPos++] = 'F';
sReturn[nReturnPos++] = bStatus ? 'F' : '0';
sReturn[nReturnPos] = '\0';
return sReturn;
}
char* RCSwitch::getCodeWordC(char sFamily, int nGroup, int nDevice, bool bStatus) {
static char sReturn[13];
int nReturnPos = 0;
int nFamily = (int)sFamily - 'a';
if ( nFamily < 0 || nFamily > 15 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4) {
return 0;
}
sReturn[nReturnPos++] = (nFamily & 1) ? 'F' : '0';
sReturn[nReturnPos++] = (nFamily & 2) ? 'F' : '0';
sReturn[nReturnPos++] = (nFamily & 4) ? 'F' : '0';
sReturn[nReturnPos++] = (nFamily & 8) ? 'F' : '0';
sReturn[nReturnPos++] = ((nDevice-1) & 1) ? 'F' : '0';
sReturn[nReturnPos++] = ((nDevice-1) & 2) ? 'F' : '0';
sReturn[nReturnPos++] = ((nGroup-1) & 1) ? 'F' : '0';
sReturn[nReturnPos++] = ((nGroup-1) & 2) ? 'F' : '0';
sReturn[nReturnPos++] = '0';
sReturn[nReturnPos++] = 'F';
sReturn[nReturnPos++] = 'F';
sReturn[nReturnPos++] = bStatus ? 'F' : '0';
sReturn[nReturnPos] = '\0';
return sReturn;
}
char* RCSwitch::getCodeWordD(char sGroup, int nDevice, bool bStatus) {
static char sReturn[13];
int nReturnPos = 0;
int nGroup = (sGroup >= 'a') ? (int)sGroup - 'a' : (int)sGroup - 'A';
if ( nGroup < 0 || nGroup > 3 || nDevice < 1 || nDevice > 3) {
return 0;
}
for (int i = 0; i < 4; i++) {
sReturn[nReturnPos++] = (nGroup == i) ? '1' : 'F';
}
for (int i = 1; i <= 3; i++) {
sReturn[nReturnPos++] = (nDevice == i) ? '1' : 'F';
}
sReturn[nReturnPos++] = '0';
sReturn[nReturnPos++] = '0';
sReturn[nReturnPos++] = '0';
sReturn[nReturnPos++] = bStatus ? '1' : '0';
sReturn[nReturnPos++] = bStatus ? '0' : '1';
sReturn[nReturnPos] = '\0';
return sReturn;
}
void RCSwitch::sendTriState(const char* sCodeWord) {
unsigned long long code = 0;
unsigned int length = 0;
for (const char* p = sCodeWord; p; p++) {
code <<= 2L;
switch (*p) {
case '0':
break;
case 'F':
code |= 1ULL;
break;
case '1':
code |= 3ULL;
break;
}
length += 2;
}
this->send(code, length);
}
static inline void safeDelayMicroseconds(unsigned long duration) {
#if defined(ESP8266) || defined(ESP32)
if (duration > 10000) {
unsigned long start = micros();
while ((micros() - start) < duration) {
yield();
}
}
else {
delayMicroseconds(duration);
}
#else
delayMicroseconds(duration);
#endif
}

void RCSwitch::send(const char * sCodeWord) {
unsigned long long code = 0;
unsigned int length = 0;
for (const char* p = sCodeWord; *p; p++) {
code <<= 1ULL;
if (*p != '0')
code |= 1ULL;
length++;
}
this->send(code, length);
}


void RCSwitch::send(uint64_t code, unsigned int length) {
if (this->nTransmitterPin == -1)
return;
#if not defined( RCSwitchDisableReceiving )
int nReceiverInterrupt_backup = nReceiverInterrupt;
if (nReceiverInterrupt_backup != -1) {
this->disableReceive();
}
#endif
for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
for (int i = 0; i < ((protocol.PreambleFactor / 2) + (protocol.PreambleFactor %2 )); i++) {
this->transmit({protocol.Preamble.high, protocol.Preamble.low});
}
if (protocol.HeaderFactor > 0) {
for (int i = 0; i < protocol.HeaderFactor; i++) {
this->transmit(protocol.Header);
}
}
for (int i = length - 1; i >= 0; i--) {
if (code & (1ULL << i))
this->transmit(protocol.one);
else
this->transmit(protocol.zero);
}
if (length == 64) {
if (nRepeat == 0) {
this->transmit(protocol.zero);
this->transmit(protocol.zero);
} else {
this->transmit(protocol.one);
this->transmit(protocol.one);
}
}
if (protocol.Guard > 0) {
digitalWrite(this->nTransmitterPin, LOW);
safeDelayMicroseconds(this->protocol.pulseLength * protocol.Guard);
}
}
digitalWrite(this->nTransmitterPin, LOW);
#if not defined( RCSwitchDisableReceiving )
if (nReceiverInterrupt_backup != -1) {
this->enableReceive(nReceiverInterrupt_backup);
}
#endif
}
void RCSwitch::transmit(HighLow pulses) {
uint8_t firstLogicLevel = (this->protocol.invertedSignal) ? LOW : HIGH;
uint8_t secondLogicLevel = (this->protocol.invertedSignal) ? HIGH : LOW;

if (pulses.high > 0) {
digitalWrite(this->nTransmitterPin, firstLogicLevel);
delayMicroseconds( this->protocol.pulseLength * pulses.high);
}
if (pulses.low > 0) {
digitalWrite(this->nTransmitterPin, secondLogicLevel);
delayMicroseconds( this->protocol.pulseLength * pulses.low);
}
}
#if not defined( RCSwitchDisableReceiving )
void RCSwitch::enableReceive(int interrupt) {
this->nReceiverInterrupt = interrupt;
this->enableReceive();
}
void RCSwitch::enableReceive() {
if (this->nReceiverInterrupt != -1) {
RCSwitch::nReceivedValue = 0;
RCSwitch::nReceivedBitlength = 0;
#if defined(RaspberryPi)
wiringPiISR(this->nReceiverInterrupt, INT_EDGE_BOTH, &handleInterrupt);
#else
attachInterrupt(this->nReceiverInterrupt, handleInterrupt, CHANGE);
#endif
}
}
void RCSwitch::disableReceive() {
#if not defined(RaspberryPi)
detachInterrupt(this->nReceiverInterrupt);
#endif
this->nReceiverInterrupt = -1;
}
bool RCSwitch::available() {
return RCSwitch::nReceivedValue != 0;
}
void RCSwitch::resetAvailable() {
RCSwitch::nReceivedValue = 0;
}
unsigned long long RCSwitch::getReceivedValue() {
return RCSwitch::nReceivedValue;
}
unsigned int RCSwitch::getReceivedBitlength() {
return RCSwitch::nReceivedBitlength;
}
unsigned int RCSwitch::getReceivedDelay() {
return RCSwitch::nReceivedDelay;
}
unsigned int RCSwitch::getReceivedProtocol() {
return RCSwitch::nReceivedProtocol;
}
unsigned int* RCSwitch::getReceivedRawdata() {
return RCSwitch::timings;
}
static inline unsigned int diff(int A, int B) {
return abs(A - B);
}

bool RECEIVE_ATTR RCSwitch::receiveProtocol(const int p, unsigned int changeCount) {
#if defined(ESP8266) || defined(ESP32)
const Protocol &pro = proto[p-1];
#else
Protocol pro;
memcpy_P(&pro, &proto[p-1], sizeof(Protocol));
#endif

unsigned long long code = 0;
unsigned int FirstTiming = 0;
if (pro.PreambleFactor > 0) {
  FirstTiming = pro.PreambleFactor + 1;
}
unsigned int BeginData = 0;
if (pro.HeaderFactor > 0) {
  BeginData = (pro.invertedSignal) ? (2) : (1);
  if (pro.HeaderFactor > 1) {
    BeginData += (pro.HeaderFactor - 1) * 2;
  }
}
const unsigned int syncLengthInPulses =  ((pro.Header.low) > (pro.Header.high)) ? (pro.Header.low) : (pro.Header.high);
unsigned int sdelay = 0;
if (syncLengthInPulses > 0) {
  sdelay = RCSwitch::timings[FirstTiming] / syncLengthInPulses;
} else if (pro.PreambleFactor > 0) {
  sdelay = RCSwitch::timings[FirstTiming-2] / pro.PreambleFactor;
}
const unsigned int delay = sdelay;
const unsigned int delayTolerance = delay * RCSwitch::nReceiveTolerance / 100;
const unsigned int firstDataTiming = BeginData + FirstTiming;
unsigned int bitChangeCount = changeCount - firstDataTiming - 1 + pro.invertedSignal;
if (bitChangeCount > 128) {
  bitChangeCount = 128;
}

for (unsigned int i = firstDataTiming; i < firstDataTiming + bitChangeCount; i += 2) {
    code <<= 1;
    if (diff(RCSwitch::timings[i], delay * pro.zero.high) < delayTolerance &&
        diff(RCSwitch::timings[i + 1], delay * pro.zero.low) < delayTolerance) {
    } else if (diff(RCSwitch::timings[i], delay * pro.one.high) < delayTolerance &&
               diff(RCSwitch::timings[i + 1], delay * pro.one.low) < delayTolerance) {
        code |= 1;
    } else {
        return false;
    }
}

if (bitChangeCount > 14) {    
    RCSwitch::nReceivedValue = code;
    RCSwitch::nReceivedBitlength = bitChangeCount / 2;
    RCSwitch::nReceivedDelay = delay;
    RCSwitch::nReceivedProtocol = p;
    return true;
}
return false;


}

void RECEIVE_ATTR RCSwitch::handleInterrupt() {

// --- УВЕЛИЧИВАЕМ СЧЕТЧИК ПРЕРЫВАНИЙ ---
RCSwitch::isrCount++;
// --------------------------------------

static unsigned int changeCount = 0;
static unsigned long lastTime = 0;
static byte repeatCount = 0;

const long time = micros();
const unsigned int duration = time - lastTime;

RCSwitch::buftimings[3]=RCSwitch::buftimings[2];
RCSwitch::buftimings[2]=RCSwitch::buftimings[1];
RCSwitch::buftimings[1]=RCSwitch::buftimings[0];
RCSwitch::buftimings[0]=duration;

if (duration > RCSwitch::nSeparationLimit ||
changeCount == 156 ||
(diff(RCSwitch::buftimings[3], RCSwitch::buftimings[2]) < 50 &&
diff(RCSwitch::buftimings[2], RCSwitch::buftimings[1]) < 50 &&
changeCount > 25)) {
if (diff(duration, RCSwitch::timings[0]) < 400 ||
changeCount == 156 ||
(diff(RCSwitch::buftimings[3], RCSwitch::timings[1]) < 50 &&
diff(RCSwitch::buftimings[2], RCSwitch::timings[2]) < 50 &&
diff(RCSwitch::buftimings[1], RCSwitch::timings[3]) < 50 &&
changeCount > 25)) {

  repeatCount++;
  if (repeatCount == 1) {
    
    bool matched = false; // <-- Флаг для отлова неизвестных пакетов

    unsigned long long thismask = 1;
    for(unsigned int i = 1; i <= numProto; i++) {
      if (RCSwitch::nReceiveProtocolMask & thismask) {
        if (receiveProtocol(i, changeCount)) {
          matched = true; // <-- Протокол подошел
          break;
        }
      }
      thismask <<= 1;
    }
    
    // --- БЛОК ОТЛОВА НЕИЗВЕСТНЫХ ПАКЕТОВ ---
    if (!matched && changeCount > 20 && !RCSwitch::unknownPacketReady) {
      RCSwitch::unknownChangeCount = changeCount;
      for (unsigned int k = 0; k < changeCount; k++) {
        RCSwitch::unknownTimings[k] = RCSwitch::timings[k];
      }
      RCSwitch::unknownPacketReady = true;
    }
    // ---------------------------------------
    
    repeatCount = 0;
  }
}
changeCount = 0;
if (diff(RCSwitch::buftimings[3], RCSwitch::buftimings[2]) < 50 &&
    diff(RCSwitch::buftimings[2], RCSwitch::buftimings[1]) < 50) {
  RCSwitch::timings[1]=RCSwitch::buftimings[3];
  RCSwitch::timings[2]=RCSwitch::buftimings[2];
  RCSwitch::timings[3]=RCSwitch::buftimings[1];
  changeCount = 4;
}


}

if (changeCount >= RCSWITCH_MAX_CHANGES) {
changeCount = 0;
repeatCount = 0;
}

if (changeCount > 0 && duration < 100) {
RCSwitch::timings[changeCount-1] += duration;
} else {
RCSwitch::timings[changeCount++] = duration;
}
lastTime = time;
}
#endif

Keeloq::Keeloq() {
_keyHigh = 0;
_keyLow = 0;
}
void Keeloq::SetKey(unsigned long keyHigh, unsigned long keyLow) {
_keyHigh = keyHigh;
_keyLow = keyLow;
}
unsigned long Keeloq::GetKey(bool HighLow) {
if (HighLow) {
return _keyHigh;
}
return _keyLow;
}
unsigned long Keeloq::Encrypt(unsigned long data) {
unsigned long x = data;
unsigned long r;
int keyBitNo, index;
unsigned long keyBitVal,bitVal;

for (r = 0; r < 528; r++) {
keyBitNo = r & 63;
if (keyBitNo < 32) {
keyBitVal = bitRead(_keyLow,keyBitNo);
} else {
keyBitVal = bitRead(_keyHigh, keyBitNo - 32);
}
index = 1 * bitRead(x, 1) + 2 * bitRead(x, 9) + 4 * bitRead(x, 20) + 8 * bitRead(x, 26) + 16 * bitRead(x, 31);
bitVal = bitRead(x, 0) ^ bitRead(x, 16) ^ bitRead(KeeLoq_NLF, index) ^ keyBitVal;
x = (x >> 1) ^ bitVal << 31;
}
return x;
}
unsigned long Keeloq::Decrypt(unsigned long data) {
unsigned long x = data;
unsigned long r;
int keyBitNo, index;
unsigned long keyBitVal,bitVal;

for (r = 0; r < 528; r++) {
keyBitNo = (15-r) & 63;
if(keyBitNo < 32) {
keyBitVal = bitRead(_keyLow,keyBitNo);
} else {
keyBitVal = bitRead(_keyHigh, keyBitNo - 32);
}
index = 1 * bitRead(x, 0) + 2 * bitRead(x, 8) + 4 * bitRead(x, 19) + 8 * bitRead(x, 25) + 16 * bitRead(x, 30);
bitVal = bitRead(x, 31) ^ bitRead(x, 15) ^ bitRead(KeeLoq_NLF, index) ^ keyBitVal;
x = (x << 1) ^ bitVal;
}
return x;
}
void Keeloq::NormLearn(unsigned long FixSN) {
unsigned long tmpFixSN;
unsigned long NewkeyHigh;
unsigned long NewkeyLow;

tmpFixSN = FixSN & 0x0FFFFFFF;
tmpFixSN |= 0x20000000;
NewkeyLow = Decrypt(tmpFixSN);
tmpFixSN = FixSN & 0x0FFFFFFF;
tmpFixSN |= 0x60000000;
NewkeyHigh = Decrypt(tmpFixSN);
_keyHigh = NewkeyHigh;
_keyLow = NewkeyLow;
}
unsigned long Keeloq::ReflectPack(unsigned long PackSrc) {
unsigned long long PackOut = 0;
for (byte i = 0; i < 32; i++) {
PackOut = PackOut << 1;
if (PackSrc & 1) {
PackOut = PackOut | 1;
}
PackSrc = PackSrc >> 1;
}
return PackOut;
}