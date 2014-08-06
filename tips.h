unsigned long getdt_usec(){
	static unsigned long last_time_usec;
	
	unsigned long now_time_usec = micros();
	unsigned long dt = now_time_usec - last_time_usec;
	last_time_usec = now_time_usec;

	return (int)max(dt,0);
}