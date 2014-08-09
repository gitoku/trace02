#include "Motor.h"
#include "Sensor.h"
#include "Servo.h"
#include "PID.h"


//ピン番号
#define ENCODER_R_INT 0
#define ENCODER_L_INT 1
#define ENCODER_R_PIN 2
#define ENCODER_L_PIN 3
#define MOTOR_R_FREE_PIN 4
#define MOTOR_R_PWM_PIN 5
#define MOTOR_L_PWM_PIN 6
#define MOTOR_L_FREE_PIN 7
#define BUZZER_PIN 8
#define LANCER_PIN 9
#define IRLED_PIN 10
#define EXTRA_PIN 11	//?
#define SW_PIN 12
#define LED_PIN 13
#define SENSOR0_PIN A0
#define SENSOR1_PIN A1
#define SENSOR2_PIN A2
#define SENSOR3_PIN A3
#define SENSOR4_PIN A4
#define SENSOR_ARRAY_PIN A5


//PID制御
#define DT  20	//制御周期
#define Kp 10000.0
#define Ki 1.0
#define Kd 0.0
PID pid(Kp,Ki,Kd, DIRECT);

//モータ
Motor motorR;
Motor motorL;

//槍
#define LANCER_CENTER 90
#define LANCER_RIGHT (90+30)
#define LANCER_LEFT (90-30)
Servo lancer;

//エンコーダー
volatile unsigned int encoder_R;
volatile unsigned int encoder_L;
void inc_pos_L(){ encoder_L++; }
void inc_pos_R(){ encoder_R++; }


void setup(){
	//シリアル通信(デバッグ用)
	Serial.begin(115200);
	Serial.println("Ready");

	//LEDスイッチ
	pinMode(SW_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);	//多分LED off

	//センサー
	Sensor::init();
	
	//エンコーダー
	pinMode(ENCODER_R_PIN, INPUT);
	pinMode(ENCODER_L_PIN, INPUT);
	attachInterrupt(ENCODER_R_INT, inc_pos_R, CHANGE);
	attachInterrupt(ENCODER_L_INT, inc_pos_L, CHANGE);
	encoder_R = 0;
	encoder_L = 0;
	
	//ランサー
	lancer.attach(LANCER_PIN);
	lancer.write(LANCER_CENTER);

	//PID制御
	pid.SetSampleTime(DT);
	pid.SetMode(AUTOMATIC);

	//起動音
	tone(BUZZER_PIN, 1000, 100);
	delay(100);
	tone(BUZZER_PIN, 1500, 100);
	delay(100);
	noTone(BUZZER_PIN);
	
	//モーター(max duty = 40)
	motorR.attach(MOTOR_R_PWM_PIN, MOTOR_R_FREE_PIN);
	motorL.attach(MOTOR_L_PWM_PIN, MOTOR_L_FREE_PIN);
	motorR.setLimit(55);
	motorL.setLimit(55);
	motorR.setMode(ON_BRAKE);
	motorL.setMode(ON_BRAKE);

	//スイッチがクリックされるまで待つ
	waitUntilClick();
	delay(500);
        
        //test();


	//センサーのキャリブレーション
	calibration();
}



#define LINE_CENTER 0	//当たり前に思うかもしれないけど、アナログだと違うみたい
#define SAFETY_STOP_TIMELIMIT 50	//[msec]

void loop(){
	static int line_lost_time = 0;	//ラインをロストした時間[msec]
	
	//センサによる測定
	Sensor::measure(BLACK);
        
        
//	int in = Sensor::getLinePosition();
        int in = Sensor::getLinePositionAnalog();

	//制御量計算
	//int out = pid.Compute(in,LINE_CENTER);
        int out = 0*in;
	
	//モータ出力
	int speed_default = 0;
	if( Sensor::getOnline() ){	//センサがライン上に存在
		motorL.write(speed_default - out);
		motorR.write(speed_default + out);
		line_lost_time = 0;
	}
	else line_lost_time += DT;	//センサを見失った
	
	//ランサー制御
	switch( Sensor::getMarkerFlag() ){
		case RIGHT: lancer.write(LANCER_RIGHT); break;
		case LEFT:  lancer.write(LANCER_LEFT);  break;
	}
	
	//ラインロストから一定時間立っていれば緊急停止
	if( line_lost_time > SAFETY_STOP_TIMELIMIT ){
		motorL.brake();
		motorR.brake();
                Serial.print("\nline lost.\nplease click switch!\n");
		waitUntilClick();
                line_lost_time = 0;
	}
	
	//デバッグ用
//        Serial.print(Sensor::right_line_end,BIN);
//        Serial_printBin(Sensor::line_status,5);
//        Serial.print(Sensor::left_line_end,BIN);

//          Serial.print("[");
//        for(int i=0;i<5;i++){
//          Serial.print(Sensor::line_status_analog[i]);
//          Serial.print("\t");
//        }
//          Serial.print("]");
//	Serial.print("\t");


//	Serial.print(in);
//	Serial.print("\t");
//	Serial.println(out);
        int pp = map(in,-100,85,0,100);
        for(int i=0;i<101;i++){ 
          if(i==pp)Serial.print("#");
          else if(i==0)Serial.print("-");
          else if(i==25)Serial.print("-");
          else if(i==50)Serial.print("-");
          else if(i==75)Serial.print("-");
          else if(i==100)Serial.print("-");
          else Serial.print("_");
        }
        Serial.print("\t[");
        Serial.print(pp-50);
        Serial.print("]");
        Serial.println();

	//制御周期DT[ms]になるように待つ
	intervalDelay_msec(DT);
}

void Serial_printBin(byte data,int num){
  for(int i=0;i<num;i++) Serial.print(bitRead(data,i),BIN);
}

//スイッチが押されるまでLED点滅させて待つ
void waitUntilClick(){
	const int BLINK_INTERVAL = 1000;	//点滅周期(msec)

	while(!digitalRead(SW_PIN)){
		digitalWrite(LED_PIN, (millis()%BLINK_INTERVAL)<(BLINK_INTERVAL/2) );
	}
	while(digitalRead(SW_PIN));
	digitalWrite(LED_PIN, LOW);
}


//前回この関数が終了してから、period[ms]経過するまで待つ
void intervalDelay_msec(int period){
	static unsigned long last_time = 0;
	unsigned long now_time = millis();

	while( (now_time - last_time) < period ) now_time = millis();
	last_time = now_time;
}


//アナログセンサ使用時の各センサの特性取得用
void calibration(){
	int maximum[5];
	int minimum[5];
	Sensor::getLineAnalog(maximum);
	Sensor::getLineAnalog(minimum);
	
	motorR.free();
	motorL.free();

	//手動で床上を走らせ、センサたちに白と黒を教えて下さい(持ち上げないこと)
	while(!digitalRead(SW_PIN)){	
		int sens_val[5];
		Sensor::getLineAnalog(sens_val);
		for(int n=0; n<5; n++){
			minimum[n] = min(minimum[n], sens_val[n]);
			maximum[n] = max(maximum[n], sens_val[n]);
                        Serial.print("[");
                        Serial.print(minimum[n]);
                        Serial.print("-");
                        Serial.print(maximum[n]);
                        Serial.print("]\t");
		} 
                Serial.println();
		tone(8, 2000, 30);
		delay(60);
	}
	while(digitalRead(SW_PIN));

	Sensor::setCharactoristics(maximum,minimum);

	tone(8, 2000, 100);
}

void test(){
  while(1){
    tone(8, 1000, 100);
    motorR.free();
    motorL.free();
    delay(5000);
    tone(8, 2000, 100);
    motorR.brake();
    motorL.brake();
    delay(5000);
  }
  
}
