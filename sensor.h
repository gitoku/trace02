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
		void calibration();
		void getLineAnalog();
		byte getMarkerDigital():
		byte getLineDigital();
		float line_pos(int *status):
		int getPosition(int *status);
		char marker_read(void);
};





#endif