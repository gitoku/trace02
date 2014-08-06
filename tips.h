unsigned long getdt_usec(){
	static unsigned long last_time_usec;
	
	unsigned long now_time_usec = micros();
	unsigned long dt = now_time_usec - last_time_usec;
	last_time_usec = now_time_usec;

	return (int)max(dt,0);
}


//[目的]
//.5周期でスイッチがちかちかして
//いつ押しても反応する

#define SW_PIN 12
#define LED_PIN 13

void blink_swich()
{
  while(!digitalRead(SW_PIN)){
    digitalWrite(LED_PIN, (millis()%1000)<500 );
  }
  while(digitalRead(SW_PIN));
}
