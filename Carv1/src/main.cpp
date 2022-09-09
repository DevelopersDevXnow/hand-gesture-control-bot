
/*
 * Author: Royston Sanctis
 * Date: 31 May 2022
 * Board: ESP32 DEVKIT V1
 * Project: Car v1
 *
 */

#include <Arduino.h>
#include <esp_now.h>

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

//Left Motor Connection

const int enA = 32;
const int in1 = 19;
const int in2 = 18;

//Right Motor Connection
const int enB = 33;
const int in3 = 17;
const int in4 = 16;


typedef struct message {
   String command;
   String alphabet;
   bool bot;
   int period;
};
struct message myMessage;



void directionControl(){


if(myMessage.command == "FORWARD"){

debugln("Move Forward");

//Move Forward
digitalWrite(in1,HIGH);
digitalWrite(in2,LOW);
digitalWrite(in3,HIGH);
digitalWrite(in4,LOW);

} 
else if(myMessage.command == "BACKWARD"){

//Move BACKWARD

debugln("Move Backward");

digitalWrite(in1,LOW);
digitalWrite(in2,HIGH);
digitalWrite(in3,LOW);
digitalWrite(in4,HIGH);


}
else if(myMessage.command == "LEFT"){

//Move Left

debugln("Move Left");

digitalWrite(in1,HIGH);
digitalWrite(in2,LOW);
digitalWrite(in3,LOW);
digitalWrite(in4,LOW);

}
else if(myMessage.command == "RIGHT"){

//Move Right

debugln("Move Right");

digitalWrite(in1,LOW);
digitalWrite(in2,LOW);
digitalWrite(in3,HIGH);
digitalWrite(in4,LOW);
}

else  if(myMessage.command == "ROLL RIGHT") {

debugln("Roll Right");

digitalWrite(in1,LOW);
digitalWrite(in2,HIGH);
digitalWrite(in3,HIGH);
digitalWrite(in4,LOW);

} 
else  if(myMessage.command == "ROLL LEFT") {

debugln("Roll Left");

digitalWrite(in1,HIGH);
digitalWrite(in2,LOW);
digitalWrite(in3,LOW);
digitalWrite(in4,HIGH);

}
else  if(myMessage.command == "STOP") {

//Turn off motors - Initial State

debugln("Stop");

digitalWrite(in1,LOW);
digitalWrite(in2,LOW);
digitalWrite(in3,LOW);
digitalWrite(in4,LOW);


}

}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

 debugln("Message received.");
   // We don't use mac to verify the sender
   // Let us transform the incomingData into our message structure
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  debug("Command:");
  debugln(myMessage.command); 
  debug("Alphabet:");
  debugln(myMessage.alphabet);
  debug("Bot:");
  debugln(myMessage.bot);

  directionControl();


}


void initESPNow(){

 
 // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}



void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:

initESPNow();

pinMode(enA, OUTPUT);
pinMode(enB, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);

//Turn off motors - Initial State

digitalWrite(in1,LOW);
digitalWrite(in2,LOW);
digitalWrite(in3,LOW);
digitalWrite(in4,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:


}