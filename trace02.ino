#include "motor.h"
#include "tips.h"
#include "PID.h"
//#include <MsTimer2.h>

#define line_center 15.0
#define speed_default 30 //30, 1.1 //40, 1.1 s=0.6
#define Kp 1.0
#define Ki 1.0
#define Kd 1.0
#define Td 0.05
#define Ti 1.0

unsigned long s_time, e_time;

unsigned int encoder_R = 0;
unsigned int encoder_L = 0;
float e[3];
float Cv[2] = {0.0, 0.0};

Motor motorL(MOTOR_R_PWM_PIN,30);
Motor motorR(MOTOR_L_PWM_PIN,30);

PID pid(Kp,Ki,Kd, DIRECT);


int sens_val[5];
int sens_ent_diff[5];
int sens_ent_line[5] = {0, 0, 0, 0, 0};
int sens_ent_none[5] = {1023, 1023, 1023, 1023, 1023};

#define dt_msec = 20;	//制御周期
#define inv_dt = 1000/dt_msec;
#define lost_line_time = 200 / dt_msec;


void inc_pos_L(){

}

void inc_pos_R(){

}

void setup(){
	Serial.begin(9600);

	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(4, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(12, INPUT);
	pinMode(13, OUTPUT);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);

	attachInterrupt(0, inc_pos_R, CHANGE);
	attachInterrupt(1, inc_pos_L, CHANGE);

	tone(8, 1000, 100);
	delay(100);
	tone(8, 1500, 100);
	delay(100);
	noTone(8);

	pid.SetSampleTime(dt_msec);
	pid.SetMode(AUTOMATIC);

	Serial.println("Ready");
	waitUntilClick();
	digitalWrite(LED_PIN, LOW);

	//MsTimer2::set(10, measureSpeed); // 10ms period
	//MsTimer2::start();
}


void loop(){
	int status;
	double in = (double)getPosition(&status);
	double out;

	pid.Compute(in,&out,0);

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

	Serial.println(Cv[0], 6);

	//制御周期dt_msec[ms]になるように待つ
	intervalDelay_msec(dt_msec);
}



