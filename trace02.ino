#include "motor.h"
#include "tips.h"
#include "PID.h"
//#include <MsTimer2.h>


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
#define ENCODER_R_PIN 2
#define ENCODER_L_PIN 3
#define ENCODER_R_INT 0
#define ENCODER_L_INT 1
unsigned int encoder_R = 0;
unsigned int encoder_L = 0;
void inc_pos_L(){
	encoder_L++;
}
void inc_pos_R(){
	encoder_R++;
}


#define BUZZER_PIN 8


void setup(){
	Serial.begin(9600);

	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(SW_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);

	pinMode(ENCODER_R_PIN, INPUT);
	pinMode(ENCODER_L_PIN, INPUT);
	attachInterrupt(ENCODER_R_INT, inc_pos_R, CHANGE);
	attachInterrupt(ENCODER_L_INT, inc_pos_L, CHANGE);

	pid.SetSampleTime(dt_msec);
	pid.SetMode(AUTOMATIC);

	tone(8, 1000, 100);
	delay(100);
	tone(8, 1500, 100);
	delay(100);
	noTone(8);

	Serial.println("Ready");
	waitUntilClick();
	digitalWrite(LED_PIN, LOW);

	//MsTimer2::set(10, measureSpeed); // 10ms period
	//MsTimer2::start();
}



#define speed_default 30 //30, 1.1 //40, 1.1 s=0.6
#define line_center 15.0
#define lost_line_time  (200 / dt_msec)

void loop(){
	int status;
	double in = (double)getPosition(&status);
	double out;
	static int lost_count = 0;

	pid.Compute(in,&out,line_center);

	if(status){
		motorL.write(speed_default - (int)out);
		motorR.write(speed_default + (int)out);
		lost_count = 0;
	}
	else lost_count++;	//ラインをロストした時間を得る
	
	
	if(lost_count > lost_line_time){
		motorL.stop();
		motorR.stop();
		waitUntilClick();
	}

	//制御周期dt_msec[ms]になるように待つ
	intervalDelay_msec(dt_msec);
}



