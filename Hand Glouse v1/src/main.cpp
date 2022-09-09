/*
 * Author: Royston Sanctis
 * Date: 27 April 2022
 * Board ESP32 DEVKIT V1
 * Project: Smart Watch
 *
 */
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include "OneButton.h"
#include <MPU6050_light.h>
#include <Preferences.h>
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


#define PIN_INPUT 23
#define BUZZ_PIN 19

const int flexPin1 = 32;
const int flexPin2 = 33;
const int flexPin3 = 35;
const int flexPin4 = 39;
const int flexPin5 = 34;

// Change these constants according to your project's design
const float VCC = 3.3;       // voltage at Ardunio 5V line
const float R_DIV = 46800.0; // resistor used to create a voltage divider

float Rflex, Vflex, Rflexc;

Preferences preferences;

float BEND_THRESHOLD = 30;

//***************Diclaration of saved preferences variables**************//

float _flatRflex1 , _flatRflex2 ,_flatRflex3 , _flatRflex4 , _flatRflex5,
_bendRflex1 , _bendRflex2 , _bendRflex3 , _bendRflex4 , _bendRflex5;

float _AccXoffset,_AccYoffset,_AccZoffset,_GyroXoffset,_GyroYoffset,_GyroZoffset;

float _Rflex1Bend,_Rflex2Bend,_Rflex3Bend,_Rflex4Bend,_Rflex5Bend, _Rflex1,_Rflex2, _Rflex3, _Rflex4, _Rflex5;

float _AngleX,_AngleY,_AngleZ;

bool flagMenu = false;

MPU6050 mpu(Wire);  
long timer = 0;

OneButton button(PIN_INPUT, true, true);

// save the millis when a press has started.
unsigned long pressStartTime;

long lastmillis = 0;
long maxtime = 30000;

int pic = 0;
int maxPics_L1 = 3;
int maxPics_L2 = 2;
int maxPics_L3 = 2;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// 'CI Logo', 128x32px
// const unsigned char myBitmap[] PROGMEM = {
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x0f, 0xff, 0xf7, 0x8f, 0x8f, 0x1e, 0x3f, 0x0f, 0x3f, 0x80, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xdf, 0xdf, 0x1e, 0x7f, 0x3f, 0xbf, 0x80, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xdf, 0xef, 0x3e, 0x7f, 0x7f, 0xbf, 0x80, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x7f, 0x7f, 0xbf, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x7f, 0xff, 0xbf, 0x3f, 0xff, 0x1e, 0x7d, 0x7f, 0x3f, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0xff, 0xe3, 0x3e, 0x3c, 0xef, 0x1e, 0x3f, 0x7c, 0x3f, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0xff, 0x9c, 0x3e, 0x3c, 0xef, 0x1e, 0x3f, 0x79, 0xff, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0xff, 0x3f, 0x1e, 0x3d, 0xef, 0x1e, 0x3c, 0x79, 0xfc, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfe, 0x77, 0x9f, 0x9f, 0xcf, 0xdf, 0xbf, 0x3f, 0xbf, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfc, 0xff, 0x8f, 0xcf, 0xcf, 0xdf, 0xbf, 0x3f, 0xbf, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfc, 0xf7, 0x83, 0x87, 0x0d, 0x8f, 0x90, 0x07, 0x2a, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfc, 0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfe, 0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x01, 0xfe, 0x77, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0xfe, 0x37, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0xff, 0x1c, 0x00, 0x04, 0x81, 0x01, 0x80, 0x31, 0x98, 0x04, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x04, 0xb1, 0x62, 0xf3, 0xd3, 0xff, 0xe0, 0x80, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x04, 0xdb, 0xb6, 0x8a, 0x93, 0x2d, 0xc5, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x04, 0x8f, 0x1f, 0x8c, 0xbd, 0x39, 0x86, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x04, 0x00, 0x03, 0x0c, 0x41, 0x31, 0x8c, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0xbf, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


// Nitte Logo 128x32px
const unsigned char myBitmap[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1c, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0e, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x27, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x23, 0x9c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x31, 0x9c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x39, 0xdc, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x38, 0xfc, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x38, 0x7c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x38, 0x3c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x38, 0x3c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x38, 0x1c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



/**
 * ESP-NOW
 * 
 * Sender
*/

// Mac address of the slave
uint8_t peer1[] = {0x78, 0xE3, 0x6D, 0x17, 0x0E, 0xA4};
uint8_t peerAll[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct message {
   String command;
   String alphabet;
   bool bot;
   int period;
};
struct message myMessage;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  debug("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  debug(macStr);
  debug(" send status:\t");
  debugln(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
} 

void header()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(22, 0);
  display.print("Smart Glouse");
  display.drawLine(0, 9, 128, 9, WHITE);
}

void header(String headerTitle)
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.print(headerTitle);
  display.drawLine(0, 9, 128, 9, WHITE);
}

void refresh()
{
  display.display();
  delay(00);
  display.clearDisplay();
}



void Display( String str){

   header();

    display.setTextSize(1);
    display.setCursor(0,15);
    display.print( str);
    // display.setTextSize(8);
    // display.setCursor(  0,   21);
    // display.print ("0.0");
    refresh();
}

void DisplayCenter(String str){

   header();

    display.setTextSize(2);
    display.setCursor(40,15);
    display.print(str);
    refresh();
}
void DisplayAlphebet( String str){


   header();

    display.setTextSize(2);
    display.setCursor(60,15);
    display.print( str);
    // display.setTextSize(8);
    // display.setCursor(  0,   21);
    // display.print ("0.0");
    refresh();
}

void restartEsp(){
// Wait 10 seconds
  debugln("Restarting in 5 seconds...");
  Display("Restarts in 5 sec");
  delay(5000);

  // Restart ESP
  ESP.restart();

}
float resistanceFlex(int16_t flexPin)
{
  Rflexc = 0.00;

  for (int cal_int = 0; cal_int < 2000; cal_int++)
  {
    if (cal_int % 125 == 0)
      Serial.print(".");

    // Read the ADC, and calculate voltage and resistance from it
    int ADCflex = analogRead(flexPin);
    Vflex = ADCflex * VCC / 4095.0;
    Rflex = R_DIV * (VCC / Vflex - 1.0);
    Rflexc += Rflex;
  }
  Rflexc /= 2000;

  return Rflexc;
}

float bendAngle(float Rflex, float flatResistance, float bendResistance)
{

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(Rflex, flatResistance, bendResistance, 0, 90.0);
  // Serial.println("Bend: " + String(angle) + " degrees");
  // Serial.println();

  return angle;
}



void IRAM_ATTR checkTicks()
{
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

void singleClick()
{
  debugln("singleClick() detected.");
  lastmillis = millis();
  if (pic >= 0 && pic < 10)
  {
    if (pic >= maxPics_L1)
    {
      pic = pic + 1 - maxPics_L1 ;
    }
    else if (pic <maxPics_L1)
    {
      pic++;
    }
  }

  if (pic >= 10 && pic < 100)
  {
    if (pic >= maxPics_L2*10)
    {
     
     pic = pic + 10 - maxPics_L2*10;
      
    }
    else if (pic < maxPics_L2*10)
    {
      pic = pic + 10;
    }
  }
  
  if (pic >= 100 && pic < 1000)
  {
    if (pic >= maxPics_L3*100)
    {
     
     pic =  pic + 100 - maxPics_L3*100;
      
    }
    else if (pic < maxPics_L3*100)
    {
      pic = pic + 100;
    }
  }

} // singleClick

// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick()
{
  debugln("doubleClick() detected.");
  lastmillis = millis();

   if(pic == 0)
   flagMenu = false;
   if (pic == 1 || pic == 2 || pic == 3)
    pic = 0;
   if (pic == 11 || pic == 21)
    pic = 1;
   if (pic == 111 || pic == 211)
    pic = 11;
  

} // doubleClick

// this function will be called when the button was pressed multiple times in a short timeframe.
void multiClick()
{
  int n = button.getNumberClicks();
  if (n == 3)
  {
    debugln("tripleClick detected.");
  }
  else if (n == 4)
  {
    debugln("quadrupleClick detected.");
  }
  else
  {
    debug("multiClick(");
    debug(n);
    debugln(") detected.");
  }

} // multiClick

// this function will be called when the button was held down for 1 second or more.
void pressStart()
{
  debugln("pressStart()");
  pressStartTime = millis() - 500; // as set in setPressTicks()
  lastmillis = millis();

  if (pic > 0 & pic < 10)
  {
    pic = pic + 10;
  }else if (pic > 10 & pic < 100)
  {
    pic = pic + 100;
  }else if (pic > 100 & pic < 1000)
  {
    pic = pic + 1000;
  }
} // pressStart()

// this function will be called when the button was released after a long hold.
void pressStop()
{
  debug("pressStop(");
  debug(millis() - pressStartTime);
  debugln(") detected.");
} // pressStop()


void beep(unsigned char delayms){
    
digitalWrite(BUZZ_PIN,HIGH);
delay(delayms);
digitalWrite(BUZZ_PIN,LOW);
delay(delayms);
digitalWrite(BUZZ_PIN,HIGH);
delay(delayms);
digitalWrite(BUZZ_PIN,LOW);
delay(delayms);
}
void tone(byte pin, int freq) {
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone
}
void noTone(byte pin) {
  tone(pin, 0);
}

void buzzTone(){

    // Sounds the buzzer at the frequency relative to the note C in Hz
    tone(BUZZ_PIN,261);    
    // Waits some time to turn off
    delay(200);
    //Turns the buzzer off
    noTone(BUZZ_PIN); 
    // Sounds the buzzer at the frequency relative to the note D in Hz   
    tone(BUZZ_PIN,293);             
    delay(200);    
    noTone(BUZZ_PIN); 
    // Sounds the buzzer at the frequency relative to the note E in Hz
    tone(BUZZ_PIN,329);      
    delay(200);
    noTone(BUZZ_PIN);     
    // Sounds the buzzer at the frequency relative to the note F in Hz
    tone(BUZZ_PIN,349);    
    delay(200);    
    noTone(BUZZ_PIN); 
    // Sounds the buzzer at the frequency relative to the note G in Hz
    tone(BUZZ_PIN,392);            
    delay(200);
    noTone(BUZZ_PIN); 
}


void menuDisplay(){
preferences.begin("sensor_values", false);

 // if (millis() >= (lastmillis + maxtime))
  // {
  //   pic = 0;
  // }



 if (pic == 0)
  {
    header();
    display.setCursor(0, 20);
    display.print("COMMAND: none");
    // display.setTextSize(8);
    // display.setCursor(  0,   21);
    // display.print ("0.0");
    refresh();

   
    
  }

  // LAYER 1////////////////////////////////////////////////////
  if (pic == 1)
  {
    header("MENU- 1/3");
    display.setCursor(0, 14); display.print(">CALIBRATION");
    display.setCursor(0, 24); display.print(" DISPLAY");
    refresh();
  }

  if (pic == 2)
  {
    header("MENU- 2/3");
    display.setCursor(0, 14); display.print(" CALIBRATION");
    display.setCursor(0, 24); display.print(">DISPLAY");
    refresh();
  }
  if (pic == 3)
  {
    header("MENU- 3/3");
    display.setCursor(0, 14); display.print(" DISPLAY");
    display.setCursor(0, 24); display.print(">PROJECT INFO");
    refresh();
  }

//LAYER 2////////////////////////////////////////////////////
if (pic == 11)
{
  header("MENU-CAL- 1/2");
  display.setCursor(0,14);  display.print (">FLEX SENSOR");
  display.setCursor(0,24);  display.print (" MPU6050");
  refresh();
}

if (pic == 21)
{
 header("MENU-CAL- 2/2");
  display.setCursor(0,14);  display.print (" FLEX SENSOR");
  display.setCursor(0,24);  display.print (">MPU6050");
  refresh();
}

if (pic == 12)
{
 header("MENU-Dis- 1/2");
  display.setCursor(0,14);  display.print (">ALPHABET");
  display.setCursor(0,24);  display.print (" PHRASE");
  refresh();
}
if (pic == 22)
{
 header("MENU-Dis- 2/2");
  display.setCursor(0,14);  display.print (" ALPHABET");
  display.setCursor(0,24);  display.print (">PHRASE");
  refresh();
}



//LAYER 3////////////////////////////////////////////////////
if (pic == 111)
{
  header("MENU-CAL-FLEX- 1/2");
  display.setCursor(0,14);  display.print (">FLAT RESISTANCE");
  display.setCursor(0,24);  display.print (" BEND RESISTANCE");
  refresh();
}

if (pic == 211)
{
 header("MENU-CAL-FLEX- 2/2");
  display.setCursor(0,14);  display.print (" FLAT RESISTANCE");
  display.setCursor(0,24);  display.print (">BEND RESISTANCE");
  refresh();
}

//Execution////////////////////////////////////////////////////

if (pic == 1111)
{
   header("Flat Resistance");
  display.setCursor(0,14);  display.print ("Calibrating Sensor...");
  refresh();
  preferences.begin("sensor_values", false);

  debugln("_flatRflex1 : " + String(resistanceFlex(flexPin1)) + " ohms");
  preferences.putFloat("_flatRflex1", Rflexc);

  debugln("_flatRflex2 : " + String(resistanceFlex(flexPin2)) + " ohms");
  preferences.putFloat("_flatRflex2", Rflexc);

  debugln("_flatRflex3 : " + String(resistanceFlex(flexPin3)) + " ohms");
  preferences.putFloat("_flatRflex3", Rflexc);

  debugln("_flatRflex4 : " + String(resistanceFlex(flexPin4)) + " ohms");
  preferences.putFloat("_flatRflex4", Rflexc);

  debugln("_flatRflex5 : " + String(resistanceFlex(flexPin5)) + " ohms");
  preferences.putFloat("_flatRflex5", Rflexc);
  debugln("Done!\n");

  delay(1000);
  display.setCursor(0,16);  display.print (" Done Calibration!");
  refresh();
  delay(1000);
  pic = 111;
  // Close the Preferences
  preferences.end();
 restartEsp();
}

if (pic == 1211)
{
   header("Bend Resistance");
  display.setCursor(0,14);  display.print ("Calibrating Sensor...");
  refresh();
  preferences.begin("sensor_values", false);

  debugln("_bendRflex1 : " + String(resistanceFlex(flexPin1)) + " ohms");
  preferences.putFloat("_bendRflex1", Rflexc);

  debugln("_bendRflex2 : " + String(resistanceFlex(flexPin2)) + " ohms");
  preferences.putFloat("_bendRflex2", Rflexc);

  debugln("_bendRflex3 : " + String(resistanceFlex(flexPin3)) + " ohms");
  preferences.putFloat("_bendRflex3", Rflexc);

  debugln("_bendRflex4 : " + String(resistanceFlex(flexPin4)) + " ohms");
  preferences.putFloat("_bendRflex4", Rflexc);

  debugln("_bendRflex5 : " + String(resistanceFlex(flexPin5)) + " ohms");
  preferences.putFloat("_bendRflex5", Rflexc);
  debugln("Done!\n");

  delay(1000);
  display.setCursor(0,16);  display.print (" Done Calibration!");
  refresh();
  delay(1000);
  pic = 111;
  // Close the Preferences
  preferences.end();

 restartEsp();
}


if (pic == 121)
{
preferences.begin("sensor_values", false);

 header("MPU6050");
  display.setCursor(0,14);  display.print ("Calibrating Sensor...");
  debugln(F("Calculating offsets, do not move MPU6050"));
  mpu.calcOffsets(true,true); // gyro and accelero
  debugln("Done!\n");
  refresh();

preferences.putFloat("_AccXoffset", mpu.getAccXoffset());
preferences.putFloat("_AccYoffset", mpu.getAccYoffset());
preferences.putFloat("_AccZoffset", mpu.getAccZoffset());

preferences.putFloat("_GyroXoffset", mpu.getGyroXoffset());
preferences.putFloat("_GyroXoffset", mpu.getGyroYoffset());
preferences.putFloat("_GyroXoffset", mpu.getGyroZoffset());

  delay(1000);
  display.setCursor(0,16);  display.print (" Done Calibration!");
  refresh();
  delay(1000);
  pic = 21;

   // Close the Preferences
  preferences.end();
   restartEsp();
}

 
}

void getSavedPreferences(){

preferences.begin("sensor_values", false);

_flatRflex1 = preferences.getFloat("_flatRflex1", 0.00);
_flatRflex2 = preferences.getFloat("_flatRflex2", 0.00);
_flatRflex3 = preferences.getFloat("_flatRflex3", 0.00);
_flatRflex4 = preferences.getFloat("_flatRflex4", 0.00);
_flatRflex5 = preferences.getFloat("_flatRflex5", 0.00);

debugln("\n");
  debugln("Rflex1Flat : " + String(_flatRflex1) + "ohms");
  debugln("Rflex2Flat : " + String(_flatRflex2) + "ohms");
  debugln("Rflex3Flat : " + String(_flatRflex3) + "ohms");
  debugln("Rflex4Flat : " + String(_flatRflex4) + "ohms");
  debugln("Rflex5Flat : " + String(_flatRflex5 )+ "ohms");
  debugln("\n\n");

_bendRflex1 = preferences.getFloat("_bendRflex1", 0.00);
_bendRflex2 = preferences.getFloat("_bendRflex2", 0.00);
_bendRflex3 = preferences.getFloat("_bendRflex3", 0.00);
_bendRflex4 = preferences.getFloat("_bendRflex4", 0.00);
_bendRflex5 = preferences.getFloat("_bendRflex5", 0.00);

 debugln("\n");
  debugln("Rflex1Bend : " + String(_bendRflex1) + "ohms");
  debugln("Rflex2Bend : " + String(_bendRflex2) + "ohms");
  debugln("Rflex3Bend : " + String(_bendRflex3) + "ohms");
  debugln("Rflex4Bend : " + String(_bendRflex4) + "ohms");
  debugln("Rflex5Bend : " + String(_bendRflex5 )+ "ohms");
  debugln("\n\n");

_AccXoffset = preferences.getFloat("_AccXoffset", 0.00);
_AccYoffset = preferences.getFloat("_AccYoffset", 0.00);
_AccZoffset = preferences.getFloat("_AccZoffset", 0.00);

_GyroXoffset = preferences.getFloat("_GyroXoffset", 0.00);
_GyroYoffset = preferences.getFloat("_GyroYoffset", 0.00);
_GyroZoffset = preferences.getFloat("_GyroZoffset", 0.00);

// Close the Preferences
  preferences.end();

}

void getReadingsFromSensor(){ 


//***************************Calculation of Flex Resistance********************//

  _Rflex1  = resistanceFlex(flexPin1);
  _Rflex2  = resistanceFlex(flexPin2);
  _Rflex3  = resistanceFlex(flexPin3);
  _Rflex4  = resistanceFlex(flexPin4);
  _Rflex5  = resistanceFlex(flexPin5);


  //***************************Calculation of Bend Angles********************//
  _Rflex1Bend = bendAngle(_Rflex1, _flatRflex1, _bendRflex1);
  _Rflex2Bend = bendAngle(_Rflex2, _flatRflex2, _bendRflex2);
  _Rflex3Bend = bendAngle(_Rflex3, _flatRflex3, _bendRflex3);
  _Rflex4Bend = bendAngle(_Rflex4, _flatRflex4, _bendRflex4);
  _Rflex5Bend = bendAngle(_Rflex5, _flatRflex5, _bendRflex5);

 //***************************Display Sensor Value in serial monitor********************//
  debugln(" \n\n");

  debugln("Rflex1 : " + String(_Rflex1) + " ohms\t" + "flex1Bend : " + String(_Rflex1Bend));
  debugln("Rflex2 : " + String(_Rflex2) + " ohms\t" + "flex2Bend : " + String(_Rflex2Bend));
  debugln("Rflex3 : " + String(_Rflex3) + " ohms\t" + "flex3Bend : " + String(_Rflex3Bend));
  debugln("Rflex4 : " + String(_Rflex4) + " ohms\t" + "flex4Bend : " + String(_Rflex4Bend));
  debugln("Rflex5 : " + String(_Rflex5) + " ohms\t" + "flex5Bend : " + String(_Rflex5Bend));


debugln(" \n\n");


  _AngleX = mpu.getAngleX();
  _AngleY = mpu.getAngleY();
  _AngleZ = mpu.getAngleZ();

if(_Rflex1Bend < -10 || _Rflex2Bend < -10 ||_Rflex3Bend < -10 || _Rflex4Bend < -10 || _Rflex5Bend < -10){

beep(200);

}


}
void initialiseSensorVaues(){

 Serial.begin(9600);
  Wire.begin();

  //*** Declare Pin Mode******//

  pinMode(BUZZ_PIN, OUTPUT);

  byte status = mpu.begin();
  (F("MPU6050 status: "));
  debugln(status);

pinMode(flexPin1, INPUT);
pinMode(flexPin2, INPUT);
pinMode(flexPin3, INPUT);
pinMode(flexPin4, INPUT);
pinMode(flexPin5, INPUT);

   debugln("One Button Example with interrupts.");

  // setup interrupt routine
  // when not registering to the interrupt the sketch also works when the tick is called frequently.
  attachInterrupt(digitalPinToInterrupt(PIN_INPUT), checkTicks, CHANGE);

  // link the xxxclick functions to be called on xxxclick event.
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);
  button.attachMultiClick(multiClick);

  button.setPressTicks(500); // that is the time when LongPressStart is called
  button.attachLongPressStart(pressStart);
  button.attachLongPressStop(pressStop);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    debugln(F("SSD1306 allocation failed"));
    // for(;;); // Don't proceed, loop forever
  }

 



//***************************Setting up offset values of MPU*********************//

mpu.setAccOffsets(_AccXoffset,_AccYoffset,_AccZoffset);
mpu.setGyroOffsets(_GyroXoffset,_GyroYoffset,_GyroZoffset);

}

void findDirection(){

//**********Here are some cammand to find the direction of the Bot*************//



}
void findAlphabet()
{


//***************Find the Alphabet using angle _AngleX, _AngleYand bend angles*********************//

 
 if (_Rflex1Bend<10.00 && _Rflex2Bend>30.00 && _Rflex3Bend>20.00 && _Rflex4Bend>20.00&& _Rflex5Bend>20.00 && _AngleY < -50 )
  {

   DisplayAlphebet("A");
   myMessage.alphabet = "A";

  }

   else if (_Rflex1Bend>50.00 && _Rflex2Bend<20.00 && _Rflex3Bend<20.00 && _Rflex4Bend<20.00 && _Rflex5Bend<20.00 && _AngleY < -50 )
  {

   DisplayAlphebet("B");
   myMessage.alphabet = "B";

  }
   else if (_Rflex1Bend < 40 && _Rflex2Bend < 50 && _Rflex3Bend < 50 && _Rflex4Bend < 35 && _Rflex5Bend < 35  && _Rflex2Bend > 10 && _Rflex3Bend > 10 && _Rflex4Bend > 10 && _Rflex5Bend > 10 && _AngleX >120 )
  {

   DisplayAlphebet("C");
  myMessage.alphabet = "C";

  }
  else if (_Rflex1Bend > 15.00 && _Rflex2Bend <10.00 && _Rflex3Bend>35.00 && _Rflex4Bend>20.00&& _Rflex5Bend>20.00 && _AngleY < -50 ){

   DisplayAlphebet("D");
  myMessage.alphabet = "D";

  }
   else if (_Rflex1Bend >50.00 && _Rflex2Bend>50.00 && _Rflex3Bend>50.00 && _Rflex4Bend>35.00&& _Rflex5Bend>60.00 && _AngleY < -50 )
  {

   DisplayAlphebet("E");
  myMessage.alphabet = "E";

  }
  else if (_Rflex1Bend >35.00 && _Rflex2Bend>35.00 && _Rflex3Bend <20.00 && _Rflex4Bend <20.00 && _Rflex5Bend < 20.00 && _AngleY < -50 )
  {

   DisplayAlphebet("F");
  myMessage.alphabet = "F";

  }
   else if (_Rflex1Bend <10.00 && _Rflex2Bend<10.00 && _Rflex3Bend>35.00 && _Rflex4Bend>15.00&& _Rflex5Bend>35.00  && _AngleX < 120  && _AngleY > -05)
  {

   DisplayAlphebet("G");
    myMessage.alphabet = "G";

  }
  else if (_Rflex1Bend <10.00 && _Rflex2Bend<10.00 && _Rflex3Bend <10.00 && _Rflex4Bend>15.00&& _Rflex5Bend>35.00 && _AngleX < 120  && _AngleY > -05.00 )
  {

   DisplayAlphebet("H");
    myMessage.alphabet = "H";

  }

  else if (_Rflex1Bend >20.00 && _Rflex2Bend>20.00 && _Rflex3Bend >20.00 && _Rflex4Bend>15.00&& _Rflex5Bend <25.00 && _AngleY < -50 )
  {

   DisplayAlphebet("I");
    myMessage.alphabet = "I";

  }
  else if (_Rflex1Bend >35.00 && _Rflex2Bend>35.00 && _Rflex3Bend >35.00 && _Rflex4Bend>15.00&& _Rflex5Bend <10.00 && _AngleY > -60)
  {

   DisplayAlphebet("J");
    myMessage.alphabet = "J";

  }

else if (_Rflex1Bend < 40.00 && _Rflex1Bend > 20.00 &&_Rflex2Bend <10.00 && _Rflex3Bend<10.00 && _Rflex4Bend>20.00&& _Rflex5Bend>50.00 && _AngleY < -50 ){

   DisplayAlphebet("K");
    myMessage.alphabet = "K";

  }
  else if (_Rflex1Bend < 10.00 && _Rflex2Bend <10.00 && _Rflex3Bend >35.00 && _Rflex4Bend>20.00&& _Rflex5Bend>20.00 && _AngleY < -50 ){

   DisplayAlphebet("L");
    myMessage.alphabet = "L";

  }
  else if (_Rflex1Bend < 15.00 &&_Rflex1Bend > 8.00&& _Rflex2Bend  >35.00 && _Rflex3Bend >35.00 && _Rflex4Bend >35.00&& _Rflex5Bend >35.00 && _AngleY < -50 ){

   DisplayAlphebet("M");
    myMessage.alphabet = "M";

  }
  else if (_Rflex1Bend < 8.00 && _Rflex2Bend  >35.00 && _Rflex3Bend >35.00 && _Rflex4Bend >35.00&& _Rflex5Bend >35.00 && _AngleY < -50 ){

   DisplayAlphebet("N");
    myMessage.alphabet = "N";

  }
  else if (_Rflex1Bend < 80 && _Rflex2Bend < 80 && _Rflex3Bend < 80 && _Rflex4Bend < 25.00 && _Rflex5Bend < 25.00  && _Rflex2Bend > 10 && _Rflex3Bend > 10 && _Rflex4Bend > 10 && _Rflex5Bend > 10 && _AngleX >120  && _AngleY < -50  )
  {

   DisplayAlphebet("O");

  }
   else if (_Rflex1Bend < 10.00 && _Rflex2Bend < 10.00 && _Rflex3Bend < 10.00  && _Rflex4Bend >15.00 && _Rflex5Bend > 35.00 && _AngleX < 10.00  && _AngleY > 10.00  )
  {

   DisplayAlphebet("P");

  }

  else if (_Rflex1Bend <10.00 && _Rflex2Bend<10.00 && _Rflex3Bend>35.00 && _Rflex4Bend>15.00&& _Rflex5Bend>35.00  && _AngleX < 120  && _AngleY > -05)
  {

   DisplayAlphebet("Q");

  }
  // else if (_Rflex1Bend >10.00 && _Rflex2Bend<5.00 && _Rflex3Bend>8.00 && _Rflex4Bend>10.00&& _Rflex5Bend>10.00  && _AngleY < -50 )
  // {

  //  DisplayAlphebet("R");
  //   myMessage.alphabet = "R";

  // }
  else if (_Rflex1Bend >20.00 && _Rflex2Bend> 60.00 &&_Rflex2Bend<75.00 && _Rflex3Bend>70.00 && _Rflex4Bend>30.00&& _Rflex5Bend>60.00  && _AngleY < -50 )
  {

   DisplayAlphebet("S");

  }
   else if (_Rflex1Bend >20.00 && _Rflex2Bend< 10.00 && _Rflex3Bend< 10.00 && _Rflex4Bend< 10.00&& _Rflex5Bend>20.00  && _AngleY < -50 )
  {

   DisplayAlphebet("W");
    myMessage.alphabet = "W";

  }
  else if (_Rflex1Bend < 10.00 && _Rflex2Bend > 15.00 && _Rflex3Bend > 15.00 && _Rflex4Bend > 15.00 && _Rflex5Bend< 10.00 && _AngleY < -50 )
  {

   DisplayAlphebet("Y");
    myMessage.alphabet = "Y";

  }
  else{
    DisplayCenter("....");
    myMessage.alphabet = " ";
  }

}// findAlphabet()

void controllBot(){
  
if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend > BEND_THRESHOLD && _Rflex3Bend > BEND_THRESHOLD && _Rflex4Bend > BEND_THRESHOLD && _Rflex5Bend> BEND_THRESHOLD && _AngleY > 30.00 ){

//Move Forward

myMessage.command = "FORWARD";
Display("FORWARD");
debugln(" Forward ");

}
else if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend > BEND_THRESHOLD && _Rflex3Bend > BEND_THRESHOLD && _Rflex4Bend > BEND_THRESHOLD && _Rflex5Bend> BEND_THRESHOLD && _AngleY < -30 ){

//Move Bckward

myMessage.command = "BACKWARD";
Display("BACKWARD");
debugln(" BACKWARD ");


}
else if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend > BEND_THRESHOLD && _Rflex3Bend > BEND_THRESHOLD && _Rflex4Bend > BEND_THRESHOLD && _Rflex5Bend> BEND_THRESHOLD && _AngleX < -30 ){

//Move Left

myMessage.command = "LEFT";
DisplayCenter("LEFT");
debugln("LEFT");


}
else if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend > BEND_THRESHOLD && _Rflex3Bend > BEND_THRESHOLD && _Rflex4Bend > BEND_THRESHOLD && _Rflex5Bend> BEND_THRESHOLD && _AngleX >30 ){

//Move Right

myMessage.command = "RIGHT";
DisplayCenter("RIGHT");
debugln("RIGHT");


}
else if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend < BEND_THRESHOLD && _Rflex3Bend < BEND_THRESHOLD && _Rflex4Bend < BEND_THRESHOLD && _Rflex5Bend< BEND_THRESHOLD && _AngleX >30 ){

//Roll Right

myMessage.command = "ROLL RIGHT";
Display("ROLL RIGHT");
debugln("ROLL RIGHT");


}else if (_Rflex1Bend > BEND_THRESHOLD && _Rflex2Bend < BEND_THRESHOLD && _Rflex3Bend < BEND_THRESHOLD && _Rflex4Bend < BEND_THRESHOLD && _Rflex5Bend< BEND_THRESHOLD && _AngleX < -30 ){

//Roll Left

myMessage.command = "ROLL LEFT";
Display("ROLL LEFT");
debugln("ROLL LEFT");


}else{

  myMessage.command = "STOP";
  DisplayCenter("STOP");
  debugln("STOP");
}



}


void splashScreen(){



 display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); // Draw white text

  display.clearDisplay(); // Make sure the display is cleared
  // Draw the bitmap:
  // drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.drawBitmap(0, 0, myBitmap, 128, 32, WHITE);

  // Update the display
  display.display();
  delay(1000);
  display.clearDisplay();

}

void espNowinitialise(){

  WiFi.disconnect();
  delay(200);
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    debugln("Error initializing ESP-NOW");
    return;
  }

    // Register the peer
  debugln("Registering a callback");
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

 // register First peer  
  memcpy(peerInfo.peer_addr, peer1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    debugln("Failed to add peer1");
    return;
  }
  // register All peer  
  memcpy(peerInfo.peer_addr, peerAll, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    debugln("Failed to add peerAll");
    return;
  }


}

void espNowSendData(){

  myMessage.alphabet = "A";
  myMessage.bot = true;
  myMessage.period = 1000;

  debugln("Send a new message");
  esp_err_t result =esp_now_send(NULL, (uint8_t *) &myMessage, sizeof(myMessage));
  if (result == ESP_OK) {
    debugln("Sent with success");
  }
  else {
    debugln("Error sending the data");
  }

  delay(200);
}

void setup()
{

getSavedPreferences();

initialiseSensorVaues();
espNowinitialise();

splashScreen();

debugln(digitalRead(PIN_INPUT));

if(digitalRead(PIN_INPUT)){

  flagMenu = true;
}

while (flagMenu)
{
   button.tick();
   mpu.update();
 
   menuDisplay();

}

//menuDisplay();
   
 

  // display.clearDisplay();
  //    display.setCursor(20, 0);
  //    display.println(F("ROYSTON"));
  //    display.display();
  //    delay(2000);
}

void loop() 
{

  button.tick();
  mpu.update();

 // menuDisplay();


 if(millis() - timer > 00){ // print data every second


getReadingsFromSensor();

debugln("\n");
debug(F("ANGLE     X: "));debug(_AngleX);
debug("\tY: ");debug(_AngleY);
debug("\tZ: ");debug(_AngleZ);
debugln(F("\n=====================================================\n"));

findAlphabet();
 controllBot();
espNowSendData();

timer = millis();


 }

 
}