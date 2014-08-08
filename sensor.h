#ifndef sensor_h
#define sensor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define IRLED_PIN 10
#define IRLED_on() digitalWrite(IRLED_PIN, LOW)
#define IRLED_off() digitalWrite(IRLED_PIN, HIGH)
enum Color {WHITE, BLACK};
enum Flag{NONE=0,RIGHT,LEFT,BOTH};

namespace Sensor {
	byte line_status;
	byte marker_status;
	int line_position;
	float line_position_analog;
	Color line_color;
	Flag flag = NONE;
	
	int line_status_analog[5];
	int maxChar[5];
	int minChar[5];
	
	void init();	//ピン初期化*
	void measure(Color _line_color);	//センサによる計測*
	
	byte getLineDigital();	//ラインセンサの状態取得
	void calcPosition();	//状態からラインの位置計算
	int getLinePosition();	//位置を取得*
	bool getOnline();	//ライン上にいるかどうか取得*

	byte getMarkerDigital();	//マーカセンサの状態取得
	void calcMarkerFlag();	//状態からフラグ算出
	Flag getMarkerFlag();	//フラグの取得*

	void getLineAnalog(int sens_val[]);	//ラインセンサの状態をアナログで取得
	void setCharactoristics(int _maxChar[],int _minChar[]);	//各ラインセンサの特性を設定*
	void calcPositionAnalog(int sens[]);	//状態からラインの位置を計算
	float getLinePositionAnalog();	//ラインの位置を取得*
};

void Sensor::init(){
	pinMode(IRLED_PIN, OUTPUT);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);
}

void Sensor::measure(Color _line_color = WHITE){
	line_color = _line_color;

	line_status = getLineDigital();
	calcPosition();

	// getLineAnalog(line_status_analog);
	// calcPositionAnalog(line_status_analog);

	marker_status = getMarkerDigital();
	calcMarkerFlag();
}

int Sensor::getLinePosition(){
	return line_position;
}

bool Sensor::getOnline(){
	if(line_status == 0b00000) return false;	//線を見失った
	if(line_status == 0b11111) return false;	//持ち上げられた時とか
	return true;
}


byte Sensor::getMarkerDigital(){
	byte marker = analogRead(A5);
	/*
	よくわかんない
	*/
	if(line_color == WHITE) return (0b11);
	if(line_color == BLACK) return ~(0b11);
}

byte Sensor::getLineDigital(){ 
	IRLED_on();
	if(line_color == WHITE) return ~(PINC) & 0b011111;
	if(line_color == BLACK) return PINC & 0b011111;
	IRLED_off();
}


void Sensor::calcPosition(){
	int pos = line_position;

	switch( line_status ){
		case 0b00001: pos = 20; break;
		case 0b00011: pos = 12; break;
		case 0b00111: pos = 10; break;
		case 0b00110: pos = 2; break;
		case 0b01110: pos = 0; break;
		case 0b01100: pos = -2; break;
		case 0b11100: pos = -10; break;
		case 0b11000: pos = -12; break;
		case 0b10000: pos = -20; break;

		case 0b00000:
			if (marker_status == 0b0100) pos = -30;
			else if (marker_status == 0b0010) pos = 30;
		break;
	}

	line_position = pos;
}


void Sensor::calcMarkerFlag(){
  if(marker_status & 0b0001) flag = RIGHT;
  if(marker_status & 0b1000) flag = LEFT;
  if(marker_status & 0b1001) flag = BOTH;
}

Flag Sensor::getMarkerFlag(){
  return flag;
}



//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓なんとなく不穏な感じの漂う関数たち↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓(アナログでセンサ取得したいようです)↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

//各センサをanalogRead
void Sensor::getLineAnalog(int sens_val[]){
	const int WAIT = 50;
	IRLED_on();
	delay(WAIT);
	for(int i=0; i<5; i++) sens_val[i] = analogRead(i);

	IRLED_off();
	delay(WAIT); 
	for(int i=0; i<5; i++) sens_val[i] -= analogRead(i);
}

//センサごとの特性(最大最小)をセット
void Sensor::setCharactoristics(int _maxChar[],int _minChar[]){
	for(int i=0;i<5;i++){ 
		maxChar[i] = _maxChar[i];
		minChar[i] = _minChar[i];
	}
}

//注意：マーカによる測定の乱れが考慮されていない
void Sensor::calcPositionAnalog(int sens[]){
	float line_pos = line_position_analog;
	
	//各センサごとに特性を考慮して正規化
	for(int i=0; i<5; i++) sens[i] = map(sens[i],minChar[i],maxChar[i],0,100);
	
	if(line_color == BLACK) {
		for(int i=0; i<5; i++) sens[i] = 100 - sens[i];
	}

	//センサごとの係数にて重み付け
	float sens_sum = 0;
	float sens_cal = 0;
	const float sens_coefficient[5] = {1, 11, 21, 31, 41};
	for(int i=0; i<5; i++){
		sens_sum += sens[i];
		sens_cal += sens[i] * sens_coefficient[i];
	}
	line_pos = sens_cal / sens_sum;
	
	//マーカセンサの情報の加味
	switch( getMarkerDigital() ){
		case 0b0010: line_pos = 0.0; break;
		case 0b0100: line_pos = 40.0; break;
	}

	line_position_analog = line_pos;
}

float Sensor::getLinePositionAnalog(){
	return line_position_analog;
}

#endif