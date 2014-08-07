#ifndef sensor_h
#define sensor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class Sensor
{
	private:
		const int LINE_SENSOR_PIN[] = {};
	public:
		Motor(int _pin);
		Motor(int _pin, int _pwm_duty_limit);
		void setPin(int _pin);
		void setLimit(int _limit);
		void setDuty(int _pwm_duty);
		void write();
		void write(int _pwm_duty);
		void write(float float_duty);
		void stop();
};

#endif


void calibration();
void getLineAnalog();
byte getMarkerDigital():
byte getLineDigital();


float line_pos(int *status):
int getPosition(int *status);
char marker_read(void);
