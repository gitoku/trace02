#ifndef motor_h
#define motor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define MOTOR_L_PWM_PIN 6
#define MOTOR_R_PWM_PIN 5
#define PWM_DUTY_MAX 255	//8bit
#define PWM_DUTY_MIN 0

class Motor
{
	private:
		int pin;
		int pwm_duty;
		int pwm_duty_limit;
	public:
		Motor(int _pin);
		Motor(int _pin, int _pwm_duty_limit);
		void setPin(int _pin);
		void setLimit(int _limit);
		void setDuty(int _pwm_duty);
		void write();
		void write(int _pwm_duty);
		void write(float float_duty);
		void stop();
};

Motor::Motor(int _pin){
	setPin(_pin);
	setLimit(PWM_DUTY_MAX);
	stop();
}

Motor::Motor(int _pin, int _pwm_duty_limit){
	setPin(_pin);
	setLimit(_pwm_duty_limit);
	stop();
}

void Motor::setPin(int _pin){
	pin = _pin;
}
void Motor::setLimit(int _limit){
	pwm_duty_limit = constrain(_limit, PWM_DUTY_MIN, PWM_DUTY_MAX);
}


void Motor::setDuty(int _pwm_duty){
	pwm_duty = constrain( _pwm_duty , PWM_DUTY_MIN, pwm_duty_limit);
}


void Motor::write(){
	analogWrite(pin, pwm_duty);
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
