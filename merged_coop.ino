/*  merged_coop
 *  Complete code used for system verification
 */

#include "thingProperties.h"
#define LAMP_MALFUNCTION_TRESH 1000 //Comparison value for ADC brightness measurement
#define LAMP_ON_AM 6                // Time that lamp will turn on in morning
#define LAMP_OFF_AM 9               // Time lamp will turn off in morning
#define LAMP_ON_PM 18               // Time lamp will turn on at night
#define LAMP_OFF_PM 21              // Time lamp will turn off at night
#define LAMP_WATTAGE 4              //Wattage rating of lamp bulb

#include "RTClib.h"
RTC_DS3231 rtc;
#include "NewPing.h"

const int dimmerPin = 6;       //Pin for PWM control of dimmer board
const int lampPhotoPin = A3;  //Photoresistor used for heat lamp

const int RED_PIN = 7;  // the Arduino pin, which connects to the IN pin of relay for the RED LED
const int YELLOW_PIN = 8;  // the Arduino pin, which connects to the IN pin of relay for the YELLOW LED
const int GREEN_PIN = 4;  // the Arduino pin, which connects to the IN pin of relay for the GREEN LED
const int ALARM_PIN = A5;  // the Arduino pin, which connects to the IN pin of relay for the BLUE LED

byte brightness_setting = 0; //Brightness setting for lamp
bool lamp_malfunction = 0;    //Indicates malfunction when high 
//int currTime[3];              //Time array, holds dummy values for simulation
float power_consumption = 0;    //Power consumed by lamp in Watt-

#define INPUT1   9  // Motor driver input 1
#define INPUT2   10 // Motor driver input 2
#define ENABLE  11  // Motor driver enable 
 
#define Low 75 // determined as the optimal speed after testing
int Speed; 

const byte photosensorPin = A0; // Environment light sensor pin

const byte thermistorPin = A1;
//Steinhart-Hart constants for temperature calculations
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 
const int R3 = 10000; //Value of resistor in series with thermistor

const byte food_data_pin = 2;
const byte water_data_pin = 3;
const float foodDensity = 0.7; //grams per mL
const float containerLength = 20;
const float containerRadius = 4.0;
const float waterContLength = 16.0;
const float waterContRadius = 4.25;
const float waterDensity = 1.0;
const int samples = 5; //Number of ultrasonic readings to take per call

bool isOpen = false;    // Current door state

NewPing food_sensor(food_data_pin, food_data_pin, 20);
NewPing water_sensor(water_data_pin, water_data_pin, 16);
 
float temp;
int currTime[3];
bool lightLevel;

float foodLevel, waterLevel;

void setup() {

  //Setup for transmitter module
  Serial.begin(57600);
  delay(1500); // Search for serial monitor
  //initProperties(); //Setup for transmitter
Serial.println("Starting code");
    // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Setup for sensors module
  pinMode(thermistorPin, INPUT);
 
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
 
  //Pin setup for door controller
  pinMode(ENABLE, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);

  //Pin setup for warning lights
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);
  Serial.println("Leaving setup");

  Speed = Low; // Set door motor speed
}

void loop() {

  //Update values and print to monitor
  getTime();
  temp = getTemp();
  lightLevel = getIfDaylight();
  foodLevel = getFoodLevel();
  waterLevel = getWaterLevel();
  printData();

  //Update cloud variables/graphs
  //ArduinoCloud.update();
  temperature = getTemp();
  waterLevel = getWaterLevel();
  foodLevel = getFoodLevel();
  powerConsumption = get_power_consumption();
  lightLevel = getIfDaylight();

  lamp_controller(); //Handles heat lamp dimming and malfunction checking
  print_lamp_data();
  getTime();
  printData(); //Print sensor values to serial monitor for testing

  doorController(); //Handles door opening and closing
  changeLights(waterLevel, foodLevel, temp, lamp_malfunction); //Handles warning light outputs
  delay(1000);

}

/* Func: print_lamp_data
 * Test bench function used to print the current test values
 * for time and heat lamp status
 */
void print_lamp_data() {
Serial.print("Power Consumption: ");
Serial.println(power_consumption);
if(lamp_malfunction)
  Serial.println("Lamp malfunction detected");
else
  Serial.println("Lamp OK");
  Serial.println("");
}

/* Func: lamp_controller
 * Determines if the heat lamp should be turned on based
 * on current time and how bright it should be
 */
void lamp_controller(){    
  if(brightness_setting == 255) //Checks lamp when fully turned on
    lamp_malfunction = check_lamp();
  if(((currTime[0] >= LAMP_ON_AM) && (currTime[0] < LAMP_OFF_AM))|| temp <= 2) { //Turns on lamp in early morning or in freezing temp
    brightness_setting = 255;
  }
  else if((currTime[0] >= LAMP_ON_PM) && (currTime[0] < LAMP_OFF_PM)) { //Turns on lamp in evening
    brightness_setting = 255;
  }
  else if(currTime[0] == LAMP_OFF_PM) { //Dim lamp until off at nighttime
    dim_lamp();  
  }
  else {
    brightness_setting = 0;   //Turn off lamp at all other times
  }
  analogWrite(dimmerPin, brightness_setting); //Set signal for brightness
    
}

/* Function: get_power_consumption
 * Calculates the power consumption of the heat lamp for the current day.
 */
float get_power_consumption() {
  float lamp_time = (LAMP_OFF_AM - LAMP_ON_AM) + (LAMP_OFF_PM - LAMP_ON_PM); //Time lamp is turned on
  float power = LAMP_WATTAGE*lamp_time; //Compute watt-hour usage
  return power;
}

/* Func: dim_lamp
 * Lowers the lamp brightness with respect to number of seconds elapsed. 
 * Should only be called when it is time for the lamp to be dimmed and turned off.
 */
void dim_lamp() {
  if(brightness_setting <= 5) //Turns off lamp when dimming is complete
    brightness_setting = 0;
  else {
    brightness_setting = 255 - 10*currTime[2]; //Reduces brightness setting by about 4% each second
  }
}

/* Func: check_lamp
 * Reads the analog voltage from the lamp photocell divider to determine if 
 * the lamp is functioning properly. This should only be called when the lamp is set at maximum brightness
 * to remain accurate. 
 */
bool check_lamp() {
  int lamp_brightness = analogRead(lampPhotoPin);  
  if(lamp_brightness < LAMP_MALFUNCTION_TRESH) //Check if ADC reading is at corresponding light threshold
    return 1;
  return 0;
}

void Forward(void){ //Starts the motor in a counterclockwise rotation in order to open the door
  analogWrite(ENABLE, Speed);
  digitalWrite(INPUT1, HIGH);
  digitalWrite(INPUT2, LOW);
}
 
void Backward(void){ //Starts the motor in a counterclockwise rotation in order to close the door 
  analogWrite(ENABLE, Speed);
  digitalWrite(INPUT1, LOW);
  digitalWrite(INPUT2, HIGH);
}
 
 
void stopped(void){//Funtion stops motor by setting both inputs to high
  digitalWrite(INPUT1, HIGH);
  digitalWrite(INPUT2, HIGH);
}

void opening(void) {
  Forward(); //opens door
  delay(300);
  stopped();

}

void closing(void) {
  Backward(); //closes door
  delay(300);
  stopped();

}

void doorController() {
    Speed=Low; // Testing showed that 75 was about as slow as it could be set while still maintaining reliable operation.
    if((currTime[0] == 19)&& (currTime[1] == 50)) {
      opening();
      delay(300);
    }
    if((currTime[0] == 5)&& (currTime[1] == 0)) {
    closing();
    }
}

/*Function: getFoodLevel
 * Determines the depth of the food in the container from
 * distance sensor and known constants.
 */
float getFoodLevel() {
  float h1 = get_food_distance(); //Distance of food from sensor
  float h2 = containerLength - h1; //Depth of food in container
  if(h2 < 0)
    h2 = 0;
  float volume = PI*containerRadius*containerRadius*h2; //Food volume in cm^3
  float mass = foodDensity*volume;
  
  return mass;
}

//Water verison of getFoodLevel
float getWaterLevel() {
    float h1 = get_water_distance(); //Distance of food from sensor
  float h2 = waterContLength - h1; //Depth of food in container
  if(h2 < 0)
    h2 = 0;
  float volume = PI*waterContRadius*waterContRadius*h2; //Food volume in cm^3
  float mass = waterDensity*volume;
  
  return mass;
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

//Water version of get_food_distance
 float get_water_distance() {
  float duration, cm;
  duration = water_sensor.ping_median(samples);
  cm = (duration / 2) * 0.0343;
  return cm;
}

/*Function: getIfDaylight
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
  Serial.print("Water Level: "  );
  Serial.println(waterLevel);

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

void onFooChange() {
  // These functions are intentionally left blank. They are what enable IoT cloud to cause changes based off of variable values
}


void onTemperatureChange() {
  // Do something
}


void onFoodLevelChange() {
  // Do something
}


void onWaterLevelChange() {
  // Do something
}


void onLightLevelChange() {
  // Do something
}


void onPowerConsumptionChange() {
  // Do something
}
 
// Function that will take each of the sensor values and determine corresponding LEDs to turn on
void changeLights(float waterLvl, float foodLvl, float temperature, bool lampMalfunction) {
  int yellow_light = waterLvl < 200 || foodLvl < 200; //Resources threshold
  int red_light = temperature <= 3; // Temperature threshold
  int green_light = !(yellow_light || red_light); // If no other lights are on, green should be on
  

//   Write all of the determined values to corresponding digital pins
  digitalWrite(RED_PIN, red_light);
  digitalWrite(YELLOW_PIN, yellow_light);
  digitalWrite(GREEN_PIN, green_light);
}

// Function to return lights back to green status
void returnNormalLights() {
  changeLights(100, 100, 100, 100);
  delay(1000);
}
