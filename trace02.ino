#include "motor.h"
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


//PID制御関係
#define dt_msec  20	//制御周期
#define Kp 1.0
#define Ki 1.0
#define Kd 1.0
PID pid(Kp,Ki,Kd, DIRECT);


//モータ(max duty = 30)
Motor motorR(MOTOR_R_PWM_PIN, MOTOR_R_FREE_PIN, 30);
Motor motorL(MOTOR_L_PWM_PIN, MOTOR_L_FREE_PIN, 30);


//エンコーダー
volatile unsigned int encoder_R;
volatile unsigned int encoder_L;
void inc_pos_L(){ encoder_L++; }
void inc_pos_R(){ encoder_R++; }


void setup(){
	//LEDスイッチ
	pinMode(SW_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
	
	//エンコーダー
	pinMode(ENCODER_R_PIN, INPUT);
	pinMode(ENCODER_L_PIN, INPUT);
	attachInterrupt(ENCODER_R_INT, inc_pos_R, CHANGE);
	attachInterrupt(ENCODER_L_INT, inc_pos_L, CHANGE);
	encoder_R = 0;
	encoder_L = 0;
	
	//PID制御
	pid.SetSampleTime(dt_msec);
	pid.SetMode(AUTOMATIC);

	//起動音
	tone(BUZZER_PIN, 1000, 100);
	delay(100);
	tone(BUZZER_PIN, 1500, 100);
	delay(100);
	noTone(BUZZER_PIN);
	
	//シリアル通信(デバッグ用)
	Serial.begin(9600);
	Serial.println("Ready");
	
	//スイッチがクリックされるまで待つ
	waitUntilClick();
	delay(500);
}



#define speed_default 30 //30, 1.1 //40, 1.1 s=0.6
#define line_center 15.0
#define lost_line_time  (200 / dt_msec)

void loop(){
	static int lost_count = 0;

	int status;
	double in = (double)getPosition(&status);
	int out = (int)pid.Compute(in,line_center);

	if(status){
		motorL.write(speed_default - out);
		motorR.write(speed_default + out);
		lost_count = 0;
	}
	else lost_count++;	//ラインをロストした時間を得る
	
	
	//ラインロストから一定時間立っていれば緊急停止
	if(lost_count > lost_line_time){
		motorL.brake();
		motorR.brake();
		waitUntilClick();
	}

	//制御周期dt_msec[ms]になるように待つ
	intervalDelay_msec(dt_msec);
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