#ifndef motor_h
#define motor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif



#define PWM_DUTY_MAX 255	//8bit
#define PWM_DUTY_MIN 0
enum Mode {ON_BRAKE=0,ON_FREE=1};


class Motor
{
	private:
		int pwm_pin;
		int free_pin;
		int pwm_duty;
		int pwm_duty_limit;
	public:
		Motor(int _pwm_pin,int _free_pin);	//ピン設定して初期化
		Motor(int _pwm_pin,int _free_pin, int _pwm_duty_limit);	//ピンとpwm_duty上限を与えて初期化
		void setLimit(int _limit);	//pwm_duty上限の設定
		void setMode(Mode mode);	//pwmモードの設定[ON-BRAKE,ON-FREE]
		void setDuty(int _pwm_duty);	//pwm_dutyの設定
		void write();	//現在のpwm_dutyを出力
		void write(int _pwm_duty);	//pwm_dutyを指定して出力
		void write(float float_duty);	//float型(0~1)でpwm_dutyを指定して出力
		void free();	//フリー
		void brake();	//ブレーキ
};

Motor::Motor(int _pwm_pin,int _free_pin){
	Motor(_pwm_pin, _free_pin, PWM_DUTY_MAX);
}

Motor::Motor(int _pwm_pin, int _free_pin, int _pwm_duty_limit){
	pwm_pin = _pwm_pin;
	free_pin = _free_pin;
	pinMode(free_pin,OUTPUT);
	setLimit(_pwm_duty_limit);
	free();
}

void Motor::setLimit(int _limit){
	pwm_duty_limit = constrain(_limit, PWM_DUTY_MIN, PWM_DUTY_MAX);
}

void Motor::setMode(Mode mode){
	digitalWrite(free_pin,mode);
}


void Motor::setDuty(int _pwm_duty){
	pwm_duty = constrain( _pwm_duty , PWM_DUTY_MIN, pwm_duty_limit);
}


void Motor::write(){
	analogWrite(pwm_pin, pwm_duty);
}
void Motor::write(int _pwm_duty){
	setDuty( _pwm_duty );
	write();
}
void Motor::write(float float_duty){
	write( (int)(float_duty * PWM_DUTY_MAX) );
}


void Motor::free(){
	setMode(ON_FREE);
	write(PWM_DUTY_MIN);
}

void Motor::brake(){
	setMode(ON_BRAKE);
	write(PWM_DUTY_MIN);
}

#endif
