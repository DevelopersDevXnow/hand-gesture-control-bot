/*
 * Author: Royston Sanctis
 * Date: 29 May 2022
 * Board:  espressif8266 Nodemcu
 * Project: Mini Bot v1
 *
 */

#include <Arduino.h>
#include <Otto.h>
Otto Otto;  
#include <SoftwareSerial.h>
#include <Servo.h>
#include <espnow.h>

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif


#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

Servo AL, AR;


/**
 * ESP-NOW
 * 
 * Reseiver
*/

typedef struct message {
   String command;
   String alphabet;
   bool bot;
   int period;
};
struct message myMessage;


int adj[]={ 0, 0,};
int pos[]={ 90,90};
int shift = 60;
int shift_inc = 10;
int shift_delay = 50;

#define NORMAL 1000
#define SLOW 2000
#define VERY_SLOW 3000
#define FAST 750
#define VERY_FAST 500
#define WAY_TO_FAST 250




#define LeftLeg D1 // left leg pin, servo[0]
#define RightLeg D2 // right leg pin, servo[1]
#define LeftFoot D3 // left foot pin, servo[2]
#define RightFoot D5 // right foot pin, servo[3]
#define Buzzer 13 //buzzer pin

#define PIN_AL D6 // left arm
#define PIN_AR D7 // right arm

void onDataReceiver(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
   debugln("Message received.");
   // We don't use mac to verify the sender
   // Let us transform the incomingData into our message structure
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  debug("Command:");
  debugln(myMessage.command); 
  debug("Sign Language:");
  debugln(myMessage.alphabet);
  debug("Bot:");
  debugln(myMessage.bot);
}
void initESPNow(){

   WiFi.disconnect();
  ESP.eraseConfig();
 
  // Wifi STA Mode
  WiFi.mode(WIFI_STA);
  // Get Mac Add
  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println("\nESP-Now Receiver");
  
  // Initializing the ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }
  
  //esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  // We can register the receiver callback function
  esp_now_register_recv_cb(onDataReceiver);
}

void move_servo(){ AL.write(pos[1]+adj[1]); AR.write(pos[2]+adj[2]);}

void forward(int T){

Otto.walk(1,T,1); // FORWARD

}
void backward(int T){

Otto.walk(1,T,-1); // BACKWARD

}

void turnLeft(int T){

Otto.turn(1,T,1); // LEFT

}

void turnRight(int T){

Otto.turn(1,T,-1); // RIGHT

}

void bendLeft(int T){

Otto.bend(1,T,1);

}

void bendRight(int T){

 Otto.bend(1,T,-1);

}

void shakeLeft(int T){

Otto.shakeLeg(1,T,-1);

}

void shakeRight(int T){

 Otto.shakeLeg(1,1000,1);
  
}


void handwaveLeft(){

    for(int angle=90; angle<90+shift; angle+=shift_inc){  pos[1] = angle;    move_servo();  delay(shift_delay);}
    for(int angle=90+shift; angle>90-shift; angle-=shift_inc) { pos[1] = angle;  move_servo(); delay(shift_delay); }
    for(int angle=90-shift; angle<90; angle+=shift_inc) {pos[1] = angle;  move_servo();   delay(shift_delay); }

}

void handwaveRight(){

   for(int angle=90; angle<90+shift; angle+=shift_inc){  pos[2] = angle;    move_servo();  delay(shift_delay);}
    for(int angle=90+shift; angle>90-shift; angle-=shift_inc) { pos[2] = angle;  move_servo(); delay(shift_delay); }
    for(int angle=90-shift; angle<90; angle+=shift_inc) {pos[2] = angle;  move_servo();   delay(shift_delay); }

}

void handsUP(){

    AL.write(160);
    AR.write(20);
    delay(shift_delay);

}

void handsDOWN(){

    AL.write(20);
    AR.write(160);
    delay(shift_delay);

}

void jump(int T){

Otto.jump(1,1000);

}

void testMoves(){

 Otto.home();
  delay(3*1000);
    forward(NORMAL);
    delay(2*1000);

    handsUP();

    delay(2*1000);
   
    handsDOWN();
   
    delay(2*1000);

    backward(NORMAL);
    
    delay(2*1000);
    turnLeft(NORMAL);

    handwaveLeft();
   
    delay(2*1000);
    turnRight(NORMAL);
    handwaveRight();
    
    delay(2*1000);
    
    bendLeft(NORMAL);

    delay(2*1000);
   
    shakeLeft(NORMAL);

    delay(2*1000);
    
    bendRight(NORMAL);

    delay(2*1000);
   
    shakeRight(NORMAL);
    

    delay(2*1000);
    handsUP();
    jump(NORMAL);

    delay(3*1000);
    handsDOWN();
  delay(2*1000);

}

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
initESPNow();
Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, Buzzer);
Otto.home();

AL.attach(PIN_AR);
AR.attach(PIN_AL);
move_servo();
delay(100);
    // Here is where your robot is configured, the servo pins connections and buzzer
Otto.home();


}

void loop() {
  // put your main code here, to run repeatedly:

   if(myMessage.command == "FORWARD"){

//Bot Move Forward.
   // forward(myMessage.period);
    forward(NORMAL);


   }else if (myMessage.command == "BACKWARD" ){

//Bot Move Backward.

   backward(NORMAL);

   }
//else {

// Otto.home();

//    }
handsUP();

 //  testMoves();

   
}