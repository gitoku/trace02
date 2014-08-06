


#define DT_MIN 1	//戻り値の最小値(０除算防止用などに)

//直前のgetdt_usec実行からの経過時間(usec)を返す
unsigned long getdt_usec(){
	static unsigned long last_time_usec;
	
	unsigned long now_time_usec = micros();
	unsigned long dt = now_time_usec - last_time_usec;
	last_time_usec = now_time_usec;

	return (int)max(dt,DT_MIN);
}




#define SW_PIN 12
#define LED_PIN 13
#define BLINK_INTERVAL 1000	//点滅周期(msec)

//スイッチが押されるまでLED点滅させて待つ
void waitUntilClick(){
  while(!digitalRead(SW_PIN)){
    digitalWrite(LED_PIN, (millis()%BLINK_INTERVAL)<(BLINK_INTERVAL/2) );
  }
  while(digitalRead(SW_PIN));
}
