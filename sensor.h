#ifndef sensor_h
#define sensor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define IRLED_PIN 10
#define IRLED_on() digitalWrite(IRLED_PIN, HIGH)
#define IRLED_off() digitalWrite(IRLED_PIN, LOW)
#define maximum(a,b) a=max(a,b)
#define minimum(a,b) a=min(a,b)

enum Color {WHITE, BLACK};
enum Flag {NONE=0,RIGHT,LEFT,BOTH};
enum DataFormat {ANALOG,DIGITAL};

namespace Sensor {
	Color line_color;

	byte line_status_digital;
	int line_status_analog[5];
	byte marker;

	int line_position_digital;
	int line_position_analog;
	bool online;

	bool right_marker;
	bool left_marker;
	bool right_line_end;
	bool left_line_end;
	
	Flag flag;

	//sensor charactoristic
	int maxChar[5];
	int minChar[5];
	int line_pos_left_end;
	int line_pos_right_end;
	

	void init(Color _line_color);	//ピン初期化*

	//センサによる計測*(以下の関数の実行を含む)
	void measure();	
		void getSensorDigital();	//すべてのセンサの状態取得
		void getLineDigital();	//状態からラインの位置計算
		void calcMarkerFlag();	//状態からフラグ算出
		void getSensorAnalogRaw(int sens_val[]);
		void getSensorAnalog();
		int calcPositionAnalog();
		void calcPositionAnalog2();	


	void refleshCharactoristics();
	void refleshAnalogSensorEnd();

	//値を取得するとき用
	int getLinePosition(DataFormat df);	//ライン位置を取得(デジタル)*
	bool getOnline();	//ライン上にいるかどうか取得(デジタル)*
	Flag getMarkerFlag();	//フラグの取得*
};

void Sensor::init(Color _line_color = WHITE){
	line_color = _line_color;
	pinMode(IRLED_PIN, OUTPUT);
	IRLED_off();
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);
	flag = NONE;

	getSensorAnalogRaw(maxChar);
	getSensorAnalogRaw(minChar);
}

void Sensor::measure(){
	getSensorDigital();
	getSensorAnalog();

	calcPositionAnalog2();
	calcMarkerFlag();
}

int Sensor::getLinePosition(DataFormat df){
	if(df == DIGITAL) return line_position_digital;
	if(df == ANALOG) return line_position_analog;
}

bool Sensor::getOnline(){
	return online;
}


void Sensor::getSensorDigital(){
	marker = (analogRead(A5)>>3);//下位3bitを切り捨て(ノイズのため)
	marker ++;	//便宜上
	marker >>= 3;	//上位4bitがそのまま各センサの入力となる
	if(line_color == BLACK) marker = (~marker)&0b1111;

	right_marker = bitRead(marker,0);
	right_line_end = bitRead(marker,1);
	left_line_end = bitRead(marker,2);
	left_marker = bitRead(marker,3);
}





void Sensor::calcMarkerFlag(){
	if(left_marker && right_marker ) flag = BOTH;
	else if(left_marker) flag = RIGHT;
	else if(right_marker) flag = LEFT;
}

Flag Sensor::getMarkerFlag(){
	return flag;
}


//各センサをanalogRead
void Sensor::getSensorAnalogRaw(int sens_val[]){
	for(int i=0; i<5; i++) sens_val[i] = analogRead(i);
}

//各センサごとに特性を考慮して正規化
void Sensor::getSensorAnalog(){
	for(int i=0; i<5; i++){ 
		line_status_analog[i] = analogRead(i);
		line_status_analog[i] = map(line_status_analog[i],minChar[i],maxChar[i],0,100);
		if(line_color == WHITE) line_status_analog[i] = 100 - line_status_analog[i];
	}
}


//センサごとの特性(最大最小)をセット
void Sensor::refleshCharactoristics(){
	int sens_val[5];
	for(int i=0;i<5;i++){ 
		sens_val[i] = analogRead(i);
		minimum(maxChar[i], sens_val[i]);
		maximum(minChar[i], sens_val[i]);
	}
}

//重み付けによる位置計算
const int sens_coefficient[5] = {1, 11, 21, 31, 41};
int Sensor::calcPositionAnalog(){
	int sens_sum = 0;
	int sens_cal = 0;

	for(int i=0; i<5; i++){
		sens_sum += line_status_analog[i];
		sens_cal += line_status_analog[i] * sens_coefficient[i];
	}
	return ( sens_cal * 100 ) / sens_sum;
}

void Sensor::refleshAnalogSensorEnd(){
	getSensorDigital();
	getSensorAnalog();

	int line_pos = calcPositionAnalog();
	if (right_line_end ) minimum(line_pos_right_end,line_pos);
	else if (left_line_end ) maximum(line_pos_left_end,line_pos);
}

void Sensor::calcPositionAnalog2(){

	int line_pos = calcPositionAnalog();
	line_pos = map(line_pos,line_pos_right_end,line_pos_left_end,-100,100);

	//マーカセンサの情報の加味
	if (right_line_end ) line_pos = -100;
	else if (left_line_end ) line_pos = 100;

	line_position_analog = line_pos;
}





// ================================================

void Sensor::getLineDigital(){
	if(line_color == WHITE) line_status_digital = ~(PINC) & 0b011111;
	else if(line_color == BLACK) line_status_digital = PINC & 0b011111;

	int pos = line_position_digital;
	
	online=true;
	switch( line_status_digital ){
		case 0b00001: pos = 20; break;
		case 0b00011: pos = 12; break;
		case 0b00111: pos = 10; break;
		case 0b00010: pos = 10; break;
		case 0b00110: pos = 5; break;
		case 0b01110: pos = 0; break;
		case 0b00100: pos = 0; break;
		case 0b01100: pos = -5; break;
		case 0b11100: pos = -10; break;
		case 0b01000: pos = -10; break;
		case 0b11000: pos = -12; break;
		case 0b10000: pos = -20; break;
		case 0b00000:
		if (left_line_end ) pos = -30;
		else if (right_line_end ) pos = 30;
		else online=false;
		break;
		default: online=false;
	}

	line_position_digital = pos;
}


#endif
