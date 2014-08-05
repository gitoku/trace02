#ifndef motor_h
#define motor_h


#define MOTOR_L_PWM_PIN 6
#define MOTOR_R_PWM_PIN 5

Class Motor
{
	private:
		int pwm_width;
		int pwm_duty_limit;
		int pwmLimit(int _pwm_width);
	public:
		Motor();
		void limitSet(int _limit);
		int pwmLimit(int _pwm_width);
		void pwmOut();
		void pwmOut(int _pwm_width);
		void pwmOut(float duty);
		void pwmStop();
}

Motor::Motor(){
	pwmLimit = PWM_DUTY_MAX;
	pwmOut = 0;
}

void Motor::limitSet(int _limit){
	pwmLimit = _limit;
}
int Motor::pwmLimit(int _pwm_width){
	return min(max(_pwm_width,0),pwm_duty_limit);
}


void Motor::pwmSet(int _pwm_width){
	pwm_width = pwmLimit( _pwm_width );
}


void Motor::pwmOut(){
	analogWrite(_PIN, pwm_width);
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
