#include <Arduino.h>
#include <Preferences.h>

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define CALIBRATE 1

const int flexPin1 = 32;
const int flexPin2 = 33;
const int flexPin3 = 35;
const int flexPin4 = 39;
const int flexPin5 = 34;

// Change these constants according to your project's design
const float VCC = 3.3;       // voltage at Ardunio 5V line
const float R_DIV = 46800.0; // resistor used to create a voltage divider
// const float flatResistance = 26600.0; // resistance when flat
// const float bendResistance = 61900.0; // resistance at 90 deg

float Rflex, Vflex, Rflexc;

Preferences preferences;

float resistanceFlex(int16_t flexPin)
{
Rflexc =0;
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
  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();

  return angle;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // pinMode(flexPin1, INPUT);
  // pinMode(flexPin2, INPUT);
  // pinMode(flexPin3, INPUT);
  // pinMode(flexPin4, INPUT);
  // pinMode(flexPin5, INPUT);

  

if(CALIBRATE ==1){
preferences.begin("glouse_values", false);

 Serial.println("Calibration");

  Serial.println("Rflex1 : " + String(resistanceFlex(flexPin1)) + " ohms");
  preferences.putFloat("Rflex1", Rflexc);

  Serial.println("Rflex2 : " + String(resistanceFlex(flexPin2)) + " ohms");
  preferences.putFloat("Rflex2", Rflexc);

  Serial.println("Rflex3 : " + String(resistanceFlex(flexPin3)) + " ohms");
  preferences.putFloat("Rflex3", Rflexc);

  Serial.println("Rflex4 : " + String(resistanceFlex(flexPin4)) + " ohms");
  preferences.putFloat("Rflex4", Rflexc);

  Serial.println("Rflex5 : " + String(resistanceFlex(flexPin5)) + " ohms");
  preferences.putFloat("Rflex5", Rflexc);

  // Close the Preferences
  preferences.end();

}

 
}

void loop()
{
   preferences.begin("glouse_values", false);
  // put your main code here, to run repeatedly:

   
 Serial.println("Rflex1 : " + String(resistanceFlex(flexPin1)) + " ohms");
Serial.println("Rflex2 : " + String(resistanceFlex(flexPin2)) + " ohms");
   Serial.println("Rflex3 : " + String(resistanceFlex(flexPin3)) + " ohms");
    Serial.println("Rflex4 : " + String(resistanceFlex(flexPin4)) + " ohms");
     Serial.println("Rflex5 : " + String(resistanceFlex(flexPin5)) + " ohms");


Serial.println(" \n\n");

  // debugln(String(preferences.getFloat("Rflex1", 0.00)));
  // debugln(String(preferences.getFloat("Rflex2", 0.00)));
  // debugln(String(preferences.getFloat("Rflex3", 0.00)));
  // debugln(String(preferences.getFloat("Rflex4", 0.00)));
  // debugln(String(preferences.getFloat("Rflex5", 0.00)));

  // debug("Rflex1Bend : " + String(bendAngle(resistanceFlex(flexPin1), preferences.getFloat("Rflex1", 0.0), preferences.getFloat("Rflex1", 0.0))) + "degrees/t");
  // debug("Rflex2Bend : " + String(bendAngle(resistanceFlex(flexPin2), preferences.getFloat("Rflex2", 0.0), preferences.getFloat("Rflex2", 0.0))) + "degrees/t");
  // debug("Rflex3Bend : " + String(bendAngle(resistanceFlex(flexPin3), preferences.getFloat("Rflex3", 0.0), preferences.getFloat("Rflex3", 0.0))) + "degrees/t");
  // debug("Rflex4Bend : " + String(bendAngle(resistanceFlex(flexPin4), preferences.getFloat("Rflex4", 0.0), preferences.getFloat("Rflex4", 0.0))) + "degrees/t");
  // debug("Rflex5Bend : " + String(bendAngle(resistanceFlex(flexPin5), preferences.getFloat("Rflex5", 0.0), preferences.getFloat("Rflex5", 0.0))) + "degrees/n/n");

  delay(2000);
}
