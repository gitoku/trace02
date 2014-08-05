
char maker_read(void)
{
  static boolean sw_r = false;
  static boolean sw_l = false;
  static byte flag = 0b000;
  static char maker = 0;
  static char last_maker = 0;
  int maker_sens_R = dig_sens_read() & 0b0001;
  int maker_sens_L = dig_sens_read() & 0b1000;
  int maker_sens_D = dig_sens_read() & 0b1001;
  
  if((maker_sens_R == 0b0001)&&(sw_r == false))
  {
    flag += 0b01;
    sw_r = true;
  }
  
  if((maker_sens_L == 0b1000)&&(sw_l == false))
  {
    flag += 0b10;
    sw_l = true;
  }
  
  if(maker_sens_D == 0b0000)
  {
    switch(flag)
    {
      case 0b01: maker = 'r'; break;
      case 0b10: maker = 'l'; break;
      case 0b11: maker = 'd'; break;
      default: maker = last_maker; 
    }
    sw_r = false;
    sw_l = false;
    flag = 0;
  } 
  last_maker = maker;
  return maker;
}

char pos_state(void)
{
  const char patturn[10] = 
  {
    'r','r','d','d','r','l','r','l','d','d'
  };
  
  const char pos_state[10] = 
  {
    'e','f','x','x','a','b','c','d','x','x'
  };
   
  
   
}
