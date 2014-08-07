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
		Motor(int _pwm_pin,int _free_pin);
		Motor(int _pwm_pin,int _free_pin, int _pwm_duty_limit);
		void setLimit(int _limit);
		void setMode(Mode mode);
		void setDuty(int _pwm_duty);
		void write();
		void write(int _pwm_duty);
		void write(float float_duty);
		void stop();
};

Motor::Motor(int _pwm_pin,int _free_pin){
	Motor(_pwm_pin, _free_pin, PWM_DUTY_MAX);
}

Motor::Motor(int _pwm_pin, int _free_pin, int _pwm_duty_limit){
	pwm_pin = _pwm_pin;
	free_pin = _free_pin;
	pinMode(free_pin,OUTPUT);
	setMode(ON_BRAKE);
	setLimit(_pwm_duty_limit);
	stop();
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


void Motor::stop(){
	setDuty(PWM_DUTY_MIN);
	write();
}

#endif
