#include "motor.h"
#include "tips.h"

#define line_center 15.0
#define lost_line_time 200
#define speed_default 30 //30, 1.1 //40, 1.1 s=0.6
#define Kp 1.0
#define Td 0.05
#define Ti 1.0

unsigned long s_time, e_time;
unsigned int lost_count = 0;
unsigned int encoder_R = 0;
unsigned int encoder_L = 0;
float e[3];
float Cv[2] = {0.0, 0.0};

Motor motorL(MOTOR_R_PWM_PIN,30);
Motor motorR(MOTOR_L_PWM_PIN,30);


//#include <MsTimer2.h>

 int sens_val[5];
 int sens_ent_diff[5];
 int sens_ent_line[5] = {0, 0, 0, 0, 0};
 int sens_ent_none[5] = {1023, 1023, 1023, 1023, 1023};


//ピンの初期化
void init_pins()
{
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
  
  //MsTimer2::set(10, measureSpeed); // 10ms period
  //MsTimer2::start();
}

void inc_pos_L(){

}

void inc_pos_R(){

}




void setup()
{
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
  
}


void loop()
{ 
  int dt = getdt_usec(); //get run period(usec)

  s_time = micros();
//  t_count_1 = s_time;
  float dCv = 0;
  static int status;
    
  e[2] = e[1];
  e[1] = e[0];
  e[0] = getPosition(&status);
 
  dCv += Kp * (e[0] - e[1]); //P_out
//  dCv += Kp * (dt/Ti) * e[0]; //I_out
  dCv += Kp * (Td/dt) * (e[0] + e[2] - 2*e[1]); //D_out
  
  
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
  int t_delay = max(0, dt *1000000 +s_time -e_time);
  delayMicroseconds(t_delay);
}

