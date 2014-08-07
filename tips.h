#ifndef tip_h
#define tip_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

//直前のgetdt_usec実行からの経過時間(usec)を返す
unsigned long getdt_usec(){
	static unsigned long last_time_usec;
	const int DT_MIN = 1;	//戻り値の最小値(０除算防止用などに)

	unsigned long now_time_usec = micros();
	unsigned long dt = now_time_usec - last_time_usec;
	last_time_usec = now_time_usec;

	return (int)max(dt,DT_MIN);
}




#define SW_PIN 12
#define LED_PIN 13

//スイッチが押されるまでLED点滅させて待つ
void waitUntilClick(){
	const int BLINK_INTERVAL = 1000;	//点滅周期(msec)

	while(!digitalRead(SW_PIN)){
		digitalWrite(LED_PIN, (millis()%BLINK_INTERVAL)<(BLINK_INTERVAL/2) );
	}
	while(digitalRead(SW_PIN));
}


void intervalDelay_msec(int period){
	static unsigned long last_time = 0;
	unsigned long now_time = millis();

	while( (now_time - last_time) < period ) now_time = millis();
	last_time = now_time;
}

void intervalDelay_usec(int period){
	static unsigned long last_time = 0;
	unsigned long now_time = micros();

	while( (now_time - last_time) < period ) now_time = micros();
	last_time = now_time;
}

#endif
