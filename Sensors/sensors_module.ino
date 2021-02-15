/* Sensor Module Prototype Code
 * Controls the sensors needed for the smart coop system and makes the results
 * available for the other modules when integrated
 * Author: Nicholas McBee
 * Created: 1/25/2021
 * Last Modified: 2/14/2021
 */

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_DS3231 rtc;

#include "NewPing.h"

const byte photosensorPin = A0;

const byte thermistorPin = A1;
//Steinhart-Hart constants
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 
const int R3 = 10000; //Value of resistor in series with thermistor

const byte food_data_pin = 3;
const byte foodEchoPin = 3;
const byte waterTriggerPin = 5;
const byte waterEchoPin = 5;
const float foodDensity = 0.7; //grams per mL
const float containerLength = 11.5;
const float containerRadius = 6.25;
const int samples = 5; //Number of ultrasonic readings to take per call

NewPing food_sensor(food_data_pin, food_data_pin);
 

int currTime[3];
bool lightLevel;
float temp;
float foodLevel, waterLevel;

void setup () {
  pinMode(thermistorPin, INPUT);
  Serial.begin(57600);
 
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
  //Update values and print to monitor
  getTime();
  temp = getTemp();
  lightLevel = getIfDaylight();
  foodLevel = getFoodLevel();
  printData();
 

  delay(1000);

}

/*Function: getFoodLevel
 * Determines the depth of the food in the container from
 * distance sensor and known constants.
 */
float getFoodLevel() {
  float h1 = get_food_distance(); //Distance of food from sensor
  float h2 = containerLength - h1; //Depth of food in container
  float volume = PI*containerRadius*containerRadius*h2; //Food volume in cm^3
  float mass = foodDensity*volume;
  
  return h1;
}

/*Function: getFoodDistance
 * Calculates the distance (in cm) of the food level from
 * the food ultrasonic sensor
 */
 float get_food_distance() {
  float duration, cm;
  duration = food_sensor.ping_median(samples);
  cm = (duration / 2) * 0.0343;
  return cm; 
 }

/*Function: getLightLevel
 * Reads analog value from photoresistor voltage divider
 * and provides a light level approximation.
 */
 bool getIfDaylight() {
  int adcVal = analogRead(photosensorPin); 
  float photoVo = 5*(float)adcVal/(1023); //Output voltage of voltage divider
  if(photoVo >= 4.17) { //Check if light level is at least 40 Lux
    return true;
  }
  return false;
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

  if(lightLevel)
    Serial.println("Daylight brightness detected");
  else
    Serial.println("Light level below daylight levels");

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
