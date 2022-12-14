#include <Arduino.h>
#include <Wire.h>

boolean recvState;
//-----------------------------------------------------------------------//   
//-----------------------------------------------------------------------// 
int16_t gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, temperature, acc_total_vector;
float angle_pitch, angle_roll, angle_yaw;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;
float elapsedTime;
long Time, timePrev, time2;
long gyro_x_cal, gyro_y_cal, gyro_z_cal;
//-----------------------------------------------------------------------// 
float pitch_PID,roll_PID,yaw_PID;
float roll_error, roll_previous_error, pitch_error, pitch_previous_error, yaw_error;
float roll_pid_p, roll_pid_d, roll_pid_i, pitch_pid_p, pitch_pid_i, pitch_pid_d, yaw_pid_p, yaw_pid_i;
double twoX_kp=5;      
double twoX_ki=0.003;
double twoX_kd=2;     
double yaw_kp=3;    
double yaw_ki=0.002;
//-----------------------------------------------------------------------// 
void setup() {
Serial.begin(9600);


//-----------------------------------------------------------------------//    
  Wire.begin();    
   Wire.setClock(400000);
  Wire.beginTransmission(0x68);                                        
  Wire.write(0x6B);                                                    
  Wire.write(0x00);                                                  
  Wire.endTransmission();         
  Wire.beginTransmission(0x68);                                      
  Wire.write(0x1C);                                                   
  Wire.write(0x10);                                                   
  Wire.endTransmission();             
  Wire.beginTransmission(0x68);                                        
  Wire.write(0x1B);                                                 
  Wire.write(0x08);                                                   
  Wire.endTransmission();  
  delay(1000);
for (int cal_int = 0; cal_int < 2000 ; cal_int ++){  
  if(cal_int % 125 == 0)Serial.print(".");                                           
  Wire.beginTransmission(0x68);                                       
  Wire.write(0x3B);                                                  
  Wire.endTransmission();                                             
  Wire.requestFrom(0x68,14);                                        
  while(Wire.available() < 14);                                        
  acc_x = Wire.read()<<8|Wire.read();                               
  acc_y = Wire.read()<<8|Wire.read();                               
  acc_z = Wire.read()<<8|Wire.read();                                 
  temperature = Wire.read()<<8|Wire.read();                           
  gyro_x = Wire.read()<<8|Wire.read();                                
  gyro_y = Wire.read()<<8|Wire.read();                                 
  gyro_z = Wire.read()<<8|Wire.read();                                              
  gyro_x_cal += gyro_x;                                              
  gyro_y_cal += gyro_y;                                             
  gyro_z_cal += gyro_z;                                              
  yield();                                                    
}
gyro_x_cal /= 2000;                                                 
gyro_y_cal /= 2000;                                                 
gyro_z_cal /= 2000;
//-----------------------------------------------------------------------//    
Time = micros();                                                                            
}

void loop(){ 
  timePrev = Time;                   
  Time = micros();  
  elapsedTime = (float)(Time - timePrev) / (float)1000000;
  Wire.beginTransmission(0x68);                                       
  Wire.write(0x3B);                                                  
  Wire.endTransmission();                                             
  Wire.requestFrom(0x68,14);                                        
  while(Wire.available() < 14);                                        
  acc_x = Wire.read()<<8|Wire.read();                               
  acc_y = Wire.read()<<8|Wire.read();                               
  acc_z = Wire.read()<<8|Wire.read();                                 
  temperature = Wire.read()<<8|Wire.read();                           
  gyro_x = Wire.read()<<8|Wire.read();                                
  gyro_y = Wire.read()<<8|Wire.read();                                 
  gyro_z = Wire.read()<<8|Wire.read(); 
gyro_x -= gyro_x_cal;                                              
gyro_y -= gyro_y_cal;                                               
gyro_z -= gyro_z_cal;                                                       
  angle_pitch += gyro_x * elapsedTime * 0.01526717557;                                 
  angle_roll += gyro_y * elapsedTime * 0.01526717557;
  angle_yaw += gyro_z * elapsedTime * 0.01526717557;
  angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               
  angle_roll -= angle_pitch * sin(gyro_z * 0.000001066); 
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       
  angle_pitch_acc += 0;                                              
  angle_roll_acc += 0;                                             
  if(set_gyro_angles){                                                
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        
  }
  else{                                                               
    angle_pitch = angle_pitch_acc;                                     
    angle_roll = angle_roll_acc;                                       
    set_gyro_angles = true;                                            
  }
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1; 
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;    

//-----------------------------------------------------------------------//


//-----------------------------------------------------------------------//
Serial.print(angle_roll_output);Serial.print("  ");
Serial.print(angle_pitch_output);Serial.print(" | ");
Serial.print(angle_yaw);Serial.print(" ");
Serial.println();

} 