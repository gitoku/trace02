void test(){
	while(1){
		tone(BUZZER_PIN, 1000, 100);
		motorR.free();
		motorL.free();
		delay(5000);
		tone(BUZZER_PIN, 2000, 100);
		motorR.brake();
		motorL.brake();
		delay(5000);
	}
}

void analogSensorPrint(){
	Serial.print("[");
	for(int i=0;i<5;i++){
		Serial.print(Sensor::line_status_analog[i]);
		Serial.print("\t");
	}
	Serial.print("]");
}

void digitalSensorPrint(){
	Serial.print(Sensor::right_line_end,BIN);
	Serial_printBin(Sensor::line_status_digital,5);
	Serial.print(Sensor::left_line_end,BIN);
}


void positionPrint(int pos){
	int pos_v = constrain(pos, 0, 100);;
	for(int i=0;i<101;i++){ 
		if(i==pos_v)Serial.print("#");
		if(i%25==0)Serial.print("-");
		else Serial.print("_");
	}
	Serial.print("\t[");
	Serial.print(pos);
	Serial.print("]");
	Serial.println();
}

void Serial_printBin(byte data,int num){
	for(int i=0;i<num;i++) Serial.print(bitRead(data,i),BIN);
}