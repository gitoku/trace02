#define KpR 0.2
#define KpL 0.2
#define Mspeed 4.0
#define motor_R 6
#define motor_L 5
#define enc_R 2
#define enc_L 3
#define default_pw 50

unsigned int temp_0;
unsigned int temp_1;

unsigned int iEnc_R;
unsigned int iEnc_L;
unsigned int uEnc_Buff_R;
unsigned int uEnc_Buff_L;
unsigned long enc_total_R = 0;
unsigned long enc_total_L = 0;

static float R_out = 0;
static float L_out = 0;

static boolean output = HIGH;
/*
void inc_pos_R(long *encoder_R)
{
  encoder_R++;
}

void inc_pos_L(long *encoder_L)
{
  encoder_L++;
}
*/

//読み取りの漏れ防止のためのbuffer
void measureSpeed() 
{
   temp_0 = encoder_R;
   temp_1 = encoder_L;
   iEnc_R = temp_0 - uEnc_Buff_R;
   iEnc_L = temp_1 - uEnc_Buff_L;
   enc_total_R += iEnc_R;
   enc_total_L += iEnc_L;
   uEnc_Buff_R = temp_0;
   uEnc_Buff_L = temp_1;
}


void m_control()
{
  R_out += KpR * (Mspeed - iEnc_R);
  L_out += KpL * (Mspeed - iEnc_L);
  
  R_out = constrain(R_out, 0, 50);
  L_out = constrain(L_out, 0, 50);
  
  analogWrite(motor_R, R_out);
  analogWrite(motor_L, L_out);
}

void m_stop()
{
//  PORTD = PORTD & 0b00001111;
  analogWrite(5, 0);
  analogWrite(6, 0);
  
}

