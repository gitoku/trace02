#ifndef sensor_h
#define sensor_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define IRLED_PIN 10
enum Color {WHITE, BLACK};

namespace Sensor {
	byte line_status;
	byte marker_status;
	int line_position;
	
	void init();
	void measure(Color line_color);
	int getLinePosition();
	bool getOnline();
	byte getMarker();
	byte getMarkerDigital(Color line_color);
	byte getLineDigital(Color line_color);
	void calcPosition();
};

void Sensor::init(){
	pinMode(IRLED_PIN, OUTPUT);
	digitalWrite(IRLED_PIN, HIGH);	//ONのつもり
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	pinMode(A4, INPUT);
	pinMode(A5, INPUT);
}

void Sensor::measure(Color line_color = WHITE){
	line_status = getLineDigital(line_color);
	marker_status = getMarkerDigital(line_color);
	calcPosition();
}

int Sensor::getLinePosition(){
	return line_position;
}

bool Sensor::getOnline(){
	if(line_status == 0b00000) return false;
	return true;
}

byte Sensor::getMarker(){
	return marker_status;
}


byte Sensor::getMarkerDigital(Color line_color){
	byte marker = analogRead(A5);
	/*
	よくわかんない
	*/
	if(line_color == WHITE) return (0b11);
	if(line_color == BLACK) return (0b11);
}

byte Sensor::getLineDigital(Color line_color){ 
	if(line_color == WHITE) return ~(PINC) & 0b011111;
	if(line_color == BLACK) return PINC & 0b011111;
}

void Sensor::calcPosition(){
	int pos = line_position;

	switch( line_status ){
		case 0b00001: pos = 20; break;
		case 0b00011: pos = 12; break;
		case 0b00111: pos = 10; break;
		case 0b00110: pos = 2; break;
		case 0b01110: pos = 0; break;
		case 0b01100: pos = -2; break;
		case 0b11100: pos = -10; break;
		case 0b11000: pos = -12; break;
		case 0b10000: pos = -20; break;

		case 0b00000:
			if (marker_status == 0b0100) pos = -30;
			else if (marker_status == 0b0010) pos = 30;
		break;
	}

	line_position = pos;
}






// ====================================================
// unsigned long t_count = 0;

// unsigned int markerInfo = 0;
// const int sens_coefficient[5] = {1, 11, 21, 31, 41}; 


// int sens_val[5];
// int sens_ent_diff[5];
// int sens_ent_line[5] = {0, 0, 0, 0, 0};
// int sens_ent_none[5] = {1023, 1023, 1023, 1023, 1023};





// //センサー初期化
// //キャリブレーション
//   //白上でボタン押す
//   //黒上でボタン押す
// void init_sensor()
// {
//   waitUntilClick();
//   digitalWrite(13, HIGH);
  
//   for(int i=0; i<100; i++)
//   {
//     for(int n=0; n<5; n++)
//     {
//       getLineAnalog();
//       if(sens_ent_line[n] < sens_val[n])
//       {
//         sens_ent_line[n] = sens_val[n];
//       }
//     }
//   }
//   tone(8, 2300, 100);
//   digitalWrite(13, LOW);
  
//   waitUntilClick();
//   digitalWrite(13, HIGH);
  
//   for(int i=0; i<100; i++)
//   {
//     for(int n=0; n<5; n++)
//     {
//       getLineAnalog();
//       if(sens_ent_none[n] > sens_val[n])
//       {
//         sens_ent_none[n] = sens_val[n];
//       }
//     }
//   }
  
//   tone(8, 2000, 100);
//   digitalWrite(13, LOW);
  
//   for(int i=0; i<5; i++)
//   {
//     sens_ent_diff[i] =  sens_ent_line[i] - sens_ent_none[i];
//   }
//   waitUntilClick();
//   digitalWrite(13, LOW);
// }


// void getLineAnalog()
// {
  
//   digitalWrite(10, LOW);
//   delay(500);
//   for(int i=0; i<5; i++)
//   {
//     sens_val[i] = analogRead(i);
//   }
  
//   digitalWrite(10, HIGH);
//   delay(500); 
//   for(int i=0; i<5; i++)
//   {
//     sens_val[i] -= analogRead(i);
//   }
// }



// float line_pos(int *status)
// {
//   float sensor_value_0[5];
//   float sensor_value_1[5];
//   float line_pos = 0.0;
//   static float last_pos = 0.0;
  
//   getLineAnalog();
//   for(int i=0; i<5; i++)
//   {
//     sensor_value_0[i] = sens_val[i] - sens_ent_none[i];
//     sensor_value_0[i] = constrain(sensor_value_0[i], 1, sens_ent_diff[i]);
//     sensor_value_1[i] = sensor_value_0[i]/sens_ent_diff[i];
//   }
//   float sens_sum = 0;
//   float sens_cal = 0;
  
//   for(int i=0; i<5; i++)
//   {
//     sens_sum += sensor_value_1[i];
//     sens_cal += sensor_value_1[i] * sens_coefficient[i];
//   }
//   line_pos = sens_cal / sens_sum;
  
//   byte markerInfo;
//   markerInfo = getMarkerDigital();
  
//    *status = 1;
//   switch(markerInfo){
//    case 0b0110: line_pos = last_pos; break;
//    case 0b0010: line_pos = 0.0; break;
//    case 0b0100: line_pos = 40.0; break;
//    case 0b0000: if(sens_sum < 0.5) 
//                 { line_pos = last_pos; *status = 0;} break;
//    default: last_pos = line_pos; 
//   }
//   return line_pos;
// }




// //マーカー読み込み->状態決定
// char marker_read(void)
// {
//   static boolean sw_r = false;
//   static boolean sw_l = false;
//   static byte flag = 0b000;
//   static char marker = 0;
//   static char last_marker = 0;
//   int marker_sens_R = getMarkerDigital() & 0b0001;
//   int marker_sens_L = getMarkerDigital() & 0b1000;
//   int marker_sens_D = getMarkerDigital() & 0b1001;
  
//   if((marker_sens_R == 0b0001)&&(sw_r == false))
//   {
//     flag += 0b01;
//     sw_r = true;
//   }
  
//   if((marker_sens_L == 0b1000)&&(sw_l == false))
//   {
//     flag += 0b10;
//     sw_l = true;
//   }
  
//   if(marker_sens_D == 0b0000)
//   {
//     switch(flag)
//     {
//       case 0b01: marker = 'r'; break;
//       case 0b10: marker = 'l'; break;
//       case 0b11: marker = 'd'; break;
//       default: marker = last_marker; 
//     }
//     sw_r = false;
//     sw_l = false;
//     flag = 0;
//   } 
//   last_marker = marker;
//   return marker;
// }




#endif