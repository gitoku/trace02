unsigned long t_count = 0;

unsigned int dig_sens_val = 0;
const int sens_coefficient[5] = {1, 11, 21, 31, 41}; 


void sensor_read()
{
  
  PORTB &= ~_BV(2);//digitalWrite(10, LOW);
  delays(500);
  for(int i=0; i<5; i++)
  {
    sens_val[i] = analogRead(i);
  }
  
  PORTB |= _BV(2);//digitalWrite(10, HIGH);
  delays(500); 
  for(int i=0; i<5; i++)
  {
    sens_val[i] -= analogRead(i);
  }
}

int dig_sens_read()
{
  unsigned int dig_sens_val;
  dig_sens_val = analogRead(A5);
  dig_sens_val = dig_sens_val >> 3;
  dig_sens_val ++;
  dig_sens_val = dig_sens_val >> 3;
  return dig_sens_val;
}

float line_pos(int *status)
{
  float sensor_value_0[5];
  float sensor_value_1[5];
  float line_pos = 0.0;
  static float last_pos = 0.0;
  
  sensor_read();
  for(int i=0; i<5; i++)
  {
    sensor_value_0[i] = sens_val[i] - sens_ent_none[i];
    sensor_value_0[i] = constrain(sensor_value_0[i], 1, sens_ent_diff[i]);
    sensor_value_1[i] = sensor_value_0[i]/sens_ent_diff[i];
  }
  float sens_sum = 0;
  float sens_cal = 0;
  
  for(int i=0; i<5; i++)
  {
    sens_sum += sensor_value_1[i];
    sens_cal += sensor_value_1[i] * sens_coefficient[i];
  }
  line_pos = sens_cal / sens_sum;
  
  unsigned int dig_sens_val;
  dig_sens_val = dig_sens_read() & 0b0110;
  
   *status = 1;
  switch(dig_sens_val){
   case 0b0110: line_pos = last_pos; break;
   case 0b0010: line_pos = 0.0; break;
   case 0b0100: line_pos = 40.0; break;
   case 0b0000: if(sens_sum < 0.5) 
                { line_pos = last_pos; *status = 0;} break;
   default: last_pos = line_pos; 
  }
  return line_pos;
}

int a_dig_read()
{
  int  sens_state = PINC;
  sens_state = sens_state & 0b011111;  
  return sens_state;
}

int dig_sens_pos(int *status)
{
  int pos = 0;
  static int last_pos;
  int dig_sens_val = dig_sens_read() & 0b0110;
  
  *status = 1;
  switch(a_dig_read()){
  
  case 0b11110: pos = 20; break;
  case 0b11100: pos = 12; break;
  case 0b11000: pos = 10; break;
  case 0b11001: pos = 2; break;
  case 0b10001: pos = 0; break;
  case 0b10011: pos = -2; break;
  case 0b00011: pos = -10; break;
  case 0b00111: pos = -12; break;
  case 0b01111: pos = -20; break;
  
  case 0b11111: 
  if (dig_sens_val == 0b0100) { pos = -30; }
  else if (dig_sens_val == 0b0010) { pos = 30; }
  else { *status = 0; pos = last_pos;} break;
  case 0b00000:  pos = last_pos; break;
  
  default: pos = last_pos; break;
  }
  last_pos = pos;
  
  return pos;
}

