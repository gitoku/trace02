#include "motor.h"
#include "tips.h"

#define line_center 15.0
#define lost_line_time 200
#define speed_default 30 //30, 1.1 //40, 1.1 s=0.6
#define Kp 1.0
#define Td 0.05
#define Ti 1.0
#define Dt 0.005

unsigned long s_time, e_time;
unsigned int lost_count = 0;
unsigned int encoder_R = 0;
unsigned int encoder_L = 0;
float e[3];
float Cv[2] = {0.0, 0.0};

Motor motorL(MOTOR_R_PWM_PIN,30);
Motor motorR(MOTOR_L_PWM_PIN,30);


void init_pins();
void blink_swich();
void init_sensor();

void sensor_read();
int dig_sens_read();
float line_pos(int *status);
int a_dig_read();
int dig_sens_pos(int *status);

char maker_read(void);



void setup()
{
  PORTD = PORTD & 0b00001111;
  Serial.begin(9600);
  init_pins();
  tone(8, 1000, 100);
  delay(100);
  tone(8, 1500, 100);
  delay(100);
  noTone(8);
  Serial.println("Ready");
  blink_swich();
  digitalWrite(13, LOW);
//  init_sensor();

  
}

void inc_pos_R()
{
  encoder_R++;
}

void inc_pos_L()
{
  encoder_L++;
}

void loop()
{
  s_time = micros();
//  t_count_1 = s_time;
  float dCv = 0;
  static int status;
    
  e[2] = e[1];
  e[1] = e[0];
  e[0] = dig_sens_pos(&status);
 
  dCv += Kp * (e[0] - e[1]); //P_out
//  dCv += Kp * (Dt/Ti) * e[0]; //I_out
  dCv += Kp * (Td/Dt) * (e[0] + e[2] - 2*e[1]); //D_out
  
  
  Cv[1] = Cv[0];
  Cv[0] = Cv[1] + dCv;
  
  
  if(status == 0 && lost_count < lost_line_time)
  {
    lost_count++;
  }
  else if(status == 1)
  {
    lost_count = 0;
  }
  
  if(lost_count >= lost_line_time)
  {
   motorL.stop();
   motorR.stop();
   blink_swich();
  }
  else
  {
    if(Cv[0] > 0)
    {
     motorL.write(speed_default - (int)Cv[0]);
     motorR.write(speed_default);
    }
    else
    {

     motorL.write(speed_default);
     motorR.write(speed_default - (int)Cv[0]);
    }
  }
 
  Serial.print(Cv[0], 6);
  Serial.print("\t");
  Serial.println(lost_count);

  e_time = micros();
  int t_delay = max(0, Dt *1000000 +s_time -e_time);
  delayMicroseconds(t_delay);
}

