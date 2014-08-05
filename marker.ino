//マーカー読み込み->状態決定
char marker_read(void)
{
  static boolean sw_r = false;
  static boolean sw_l = false;
  static byte flag = 0b000;
  static char marker = 0;
  static char last_marker = 0;
  int marker_sens_R = dig_sens_read() & 0b0001;
  int marker_sens_L = dig_sens_read() & 0b1000;
  int marker_sens_D = dig_sens_read() & 0b1001;
  
  if((marker_sens_R == 0b0001)&&(sw_r == false))
  {
    flag += 0b01;
    sw_r = true;
  }
  
  if((marker_sens_L == 0b1000)&&(sw_l == false))
  {
    flag += 0b10;
    sw_l = true;
  }
  
  if(marker_sens_D == 0b0000)
  {
    switch(flag)
    {
      case 0b01: marker = 'r'; break;
      case 0b10: marker = 'l'; break;
      case 0b11: marker = 'd'; break;
      default: marker = last_marker; 
    }
    sw_r = false;
    sw_l = false;
    flag = 0;
  } 
  last_marker = marker;
  return marker;
}

//マーカ判断後の動作定義
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
