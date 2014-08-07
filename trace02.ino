#include "motor.h"
#include "tips.h"
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




int sens_val[5];
int sens_ent_diff[5];
int sens_ent_line[5] = {0, 0, 0, 0, 0};
int sens_ent_none[5] = {1023, 1023, 1023, 1023, 1023};




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

	Serial.println("Ready");
	waitUntilClick();
	digitalWrite(LED_PIN, LOW);

	//MsTimer2::set(10, measureSpeed); // 10ms period
	//MsTimer2::start();
}


void loop(){
	const int dt_msec = 20;	//制御周期
	const int inv_dt = 1000/dt_msec;
	const int lost_line_time = 200 / dt_msec;

	static unsigned int lost_count = 0;
	static int last_in = 0;
	static int total_in = 0;
	
	int status;
	int in = getPosition(&status);
	total_in += in;

	int out = 0;
	out += Kp * in; //P_out
	out += Ki * (total_in * inv_dt); //I_out
	out += Kd * (in - last_in) / inv_dt; //D_out


	if(status){
		if(Cv[0] > 0){
			motorL.write(speed_default - (int)Cv[0]);
			motorR.write(speed_default);
		}
		else{
			motorL.write(speed_default);
			motorR.write(speed_default - (int)Cv[0]);
		}
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



