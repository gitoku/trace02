#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifndef trace_test_h
#define trace_test_h

void init_pins();
void blink_swich();
void init_sensor();
void delays(unsigned int t_delay);

void sensor_read();
int dig_sens_read();
float line_pos(int *status);
int a_dig_read();
int dig_sens_pos(int *status);

char maker_read(void);

void fnrot_R();
void fnrot_L();
void measureSpeed();
void m_controle();
void m_stop();
void pw_controle(int pw_r, int pw_l);

#endif
