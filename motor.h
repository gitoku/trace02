#ifndef motor_h
#define motor_h


#define MOTOR_L_PWM_PIN 6
#define MOTOR_R_PWM_PIN 5
#define PWM_DUTY_MAX 255

class Motor
{
	private:
		int pin;
		int pwm_width;
		int pwm_duty_limit;
		int pwmLimit(int _pwm_width);
	public:
		Motor(int _pin);
		void pinSet(int _pin);
		void limitSet(int _limit);
		void pwmSet(int _pwm_width);
		void pwmOut();
		void pwmOut(int _pwm_width);
		void pwmOut(float duty);
		void pwmStop();
};

Motor::Motor(int _pin){
	pwm_duty_limit = PWM_DUTY_MAX;
	pwm_width = 0;
	pinSet(_pin);
}

void Motor::pinSet(int _pin){
	pin = _pin;
}
void Motor::limitSet(int _limit){
	pwm_duty_limit = _limit;
}
int Motor::pwmLimit(int _pwm_width){
	return min(max(_pwm_width,0),pwm_duty_limit);
}


void Motor::pwmSet(int _pwm_width){
	pwm_width = pwmLimit( _pwm_width );
}


void Motor::pwmOut(){
	analogWrite(pin, pwm_width);
}
void Motor::pwmOut(int _pwm_width){
	pwmSet( _pwm_width );
	pwmOut();
}
void Motor::pwmOut(float duty){
	pwmOut( (int)(duty * PWM_DUTY_MAX) );
}


void Motor::pwmStop(){
	pwmSet(0);
	pwmOut();
}

#endif
