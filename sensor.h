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

enum Color {WHITE, BLACK};
enum Flag {NONE=0,RIGHT,LEFT,BOTH};
enum DataFormat {ANALOG,DIGITAL};

namespace Sensor {
	byte line_status;
	int line_position_digital;
	byte marker;
	int line_position_analog;
	bool online;
	Color line_color;
	Flag flag;
	bool right_marker;
	bool right_line_end;
	bool left_line_end;
	bool left_marker;
	
	int line_status_analog[5];
	int maxChar[5];
	int minChar[5];
	int line_pos_left_end;
	int line_pos_right_end;
	

	void init();	//ピン初期化*

	//センサによる計測*(以下の関数の実行を含む)
	void measure(Color _line_color);	
		void getSensorDigital();	//すべてのセンサの状態取得
		//ライン(デジタル)
		void calcPositionDigital();	//状態からラインの位置計算
		//マーカー
		void calcMarkerFlag();	//状態からフラグ算出
		//ライン(アナログ)
		void getSensorAnalogRaw(int sens_val[]);	//ラインセンサの状態をアナログで取得
		void getSensorAnalog();
		void calcPositionAnalog();	//状態からラインの位置を計算
		void refleshCharactoristics();

	//値を取得するとき用
	int getLinePosition(DataFormat df);	//ライン位置を取得(デジタル)*
	bool getOnline();	//ライン上にいるかどうか取得(デジタル)*
	Flag getMarkerFlag();	//フラグの取得*
};

void Sensor::init(){
	pinMode(IRLED_PIN, OUTPUT);
	IRLED_off();
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);
	flag = NONE;
}

void Sensor::measure(Color _line_color = WHITE){
	line_color = _line_color;

	getSensorDigital();
	getSensorAnalog();

	calcPositionDigital();
	calcPositionAnalog();

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

	if(line_color == WHITE) line_status = ~(PINC) & 0b011111;
	else if(line_color == BLACK) line_status = PINC & 0b011111;
}


void Sensor::calcPositionDigital(){
	int pos = line_position_digital;
	
	online=true;
	switch( line_status ){
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
		maxChar[i] = min(maxChar[i], sens_val[i]);
		minChar[i] = max(minChar[i], sens_val[i]);
	}
}

//注意：マーカによる測定の乱れが考慮されていない
void Sensor::calcPositionAnalog(){
	int line_pos;
	


	//センサごとの係数にて重み付け
	float sens_sum = 0;
	float sens_cal = 0;
	const float sens_coefficient[5] = {1, 11, 21, 31, 41};
	for(int i=0; i<5; i++){
		sens_sum += line_status_analog[i];
		sens_cal += line_status_analog[i] * sens_coefficient[i];
	}
	line_pos = sens_cal / sens_sum;
	line_pos *= 100;
	line_pos = map(line_pos,line_pos_right_end,line_pos_left_end,-100,100);

	//マーカセンサの情報の加味
	if (right_line_end ) line_pos = -100;
	else if (left_line_end ) line_pos = 100;

	line_position_analog = line_pos;
}

#endif
