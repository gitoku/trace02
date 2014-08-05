#include <MsTimer2.h>

 int sens_val[5];
 int sens_ent_diff[5];
 int sens_ent_line[5] = {0, 0, 0, 0, 0};
 int sens_ent_none[5] = {1023, 1023, 1023, 1023, 1023};

void init_pins()
{
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
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

void blink_swich()
{
  boolean output = HIGH;
  int count = 0;
  for(;;)
  {
    digitalWrite(13, output);
    delay(1000);
    count ++;
    if(count > 500)
    {
      count = 0;
      output = !output;
    }
    if(digitalRead(12) == HIGH)
    {
      break;
    }
  }
  while(digitalRead(12) == HIGH);
}

void init_sensor()
{
  blink_swich();
  digitalWrite(13, HIGH);
  
  for(int i=0; i<100; i++)
  {
    for(int n=0; n<5; n++)
    {
      sensor_read();
      if(sens_ent_line[n] < sens_val[n])
      {
        sens_ent_line[n] = sens_val[n];
      }
    }
  }
  tone(8, 2300, 100);
  digitalWrite(13, LOW);
  
  blink_swich();
  digitalWrite(13, HIGH);
  
  for(int i=0; i<100; i++)
  {
    for(int n=0; n<5; n++)
    {
      sensor_read();
      if(sens_ent_none[n] > sens_val[n])
      {
        sens_ent_none[n] = sens_val[n];
      }
    }
  }
  
  tone(8, 2000, 100);
  digitalWrite(13, LOW);
  
  for(int i=0; i<5; i++)
  {
    sens_ent_diff[i] =  sens_ent_line[i] - sens_ent_none[i];
  }
  blink_swich();
  digitalWrite(13, LOW);
}

