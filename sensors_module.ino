// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_DS3231 rtc;

const byte photosensorPin = A0;

const byte thermistorPin = A1;
//Steinhart-Hart constants
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 
const int R3 = 10000; //Value of resistor in series with thermistor

const byte foodTriggerPin = 3;
const byte foodEchoPin = 4;
const byte waterTriggerPin = 5;
const byte waterEchoPin = 5;
const float foodDensity = 0.7; //grams per mL
const float containerLength = 13.0;
const float containerRadius = 6.25;
//const float foodCapacity 




int currTime[3];
float lightLevel;
float temp;
float foodLevel, waterLevel;

void setup () {
  pinMode(thermistorPin, INPUT);
  Serial.begin(57600);

  pinMode(foodTriggerPin, OUTPUT);
  pinMode(foodEchoPin, INPUT); 

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

}

void loop () {
  getTime();
  temp = getTemp();
  lightLevel = getLightLevel();
  foodLevel = getFoodLevel();
  printData();
 

  delay(1000);
  

 

   
}

float getFoodLevel() {
  float h1 = getFoodDistance(); //Distance of food from sensor
  float h2 = containerLength - h1; //Depth of food in container
  float volume = PI*containerRadius*containerRadius*h2; //Food volume in cm^3
  float mass = foodDensity*volume;
  
  return h1;
}

/*Function: getFoodDistance
 * Calculates the distance (in cm) of the food level from
 * the food ultrasonic sensor
 */
 float getFoodDistance() {
  float duration, cm;
  
  //Sends a 10uS pulse out from sensor
  digitalWrite(foodTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(foodTriggerPin, LOW);

  //Times how long it took for pulse to bounce back
  duration = pulseIn(foodEchoPin, HIGH); 
  cm = 0.017*duration; //Convert time delay to distance
  return cm;
 }

/*Function: getLightLevel
 * Reads analog value from photoresistor voltage divider
 * and provides a light level approximation.
 */
 float getLightLevel() {
  int adcVal = analogRead(photosensorPin); 
  float photoVo = 5*(float)adcVal/(1023); //Output voltage of voltage divider
  return photoVo;
 }

/*Function: getTemp
 *Reads analog value from thermistor voltage
 *divier and calculates the temperature in Celsius.
 */
float getTemp() {
  int adcVal = analogRead(thermistorPin);
  float rTherm = R3*(1023.0/(float)adcVal - 1.0); //Current thermistor resistance
  float logRtherm = log(rTherm);
  //Temperature in Kelvin from Steinhart equation:
  float tempK = (1.0/(c1 + c2*logRtherm + c3*logRtherm*logRtherm*logRtherm)); 
  return (tempK - 273.15); //Return temp converted to Celsius
  
 }

/*Function: printData
 *Prints the sensor values to the 
 *serial monitor for testing and debugging.
 */
 void printData() {
  Serial.print("Time: ");
  Serial.print(currTime[0]);
  Serial.print(":");
  Serial.print(currTime[1]);
  Serial.print(":");
  Serial.println(currTime[2]);

  Serial.print("Temperature (C): ");
  Serial.println(temp); 

  Serial.print("Light Level: ");
  Serial.println(lightLevel); 

  Serial.print("Food Level: ");
  Serial.println(foodLevel); 

  Serial.println(" ");
 }

/*Function: getTime
 *Collects current time info from 
 *the RTC module and updates the
 *current time array.
 */
void getTime() {
  DateTime now = rtc.now();
  
  currTime[0] = now.hour();
  currTime[1] = now.minute();
  currTime[2] = now.second();
  
}
