#define cc1101_debug_service_h
#include <Arduino.h>

class CC1101_debug
{
private:
void print_register(byte b);
void print_line(byte l);
void print_string(String s);
void show_version(void);
void show_register(void);

void mhzQ(void);
void get_mhz(void);
void write_mhz(float f);

void Split_MDMCFG1(void);

void chanQ(void);
void write_chan(int c);

void chspQ(void);
void show_chsp(void);
void write_chsp(float f);
//void get_chsp(float f);

void rxbwQ(void);
void show_rxbw(void);
void write_rxbw(float f);
//void get_rxbw(float f);

void Split_MDMCFG4(void);

void drateQ(void);
void write_drate(float d);
void show_drate(void);
//void get_drate(float d);

void deviationQ(void);
void write_deviation(float d);
void show_deviation(void);
//void get_deviation(float d);

void get_slot(int s);
void show_slot(void);

public:

void debug(void);
void save_slot(int s);
void load_slot(int s);

};

extern CC1101_debug cc1101_debug;
