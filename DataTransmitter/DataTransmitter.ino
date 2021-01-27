#include "thingProperties.h"

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}


float mockWaterLvl = 0;
float mockFoodLvl = 0;
float mockTemperature = 0;
int mockLightLvl = 0;
int mockPowerLvl = 0;

float mockGetWaterLvl() {
  mockWaterLvl += 0.1;
  return mockWaterLvl;
}

float mockGetFoodLvl() {
  mockFoodLvl += 0.2;
  return mockFoodLvl;
}

float mockGetTemperature() {
  mockTemperature += .3;
  return mockTemperature;
}

int mockGetPowerConsumption() {
  mockLightLvl += 1;
  return mockLightLvl;
}

int mockGetLightLevel() {
  mockPowerLvl += 2;
  return mockPowerLvl;
}
void loop() {
  //Update Variables in the cloud every 5 seconds
  ArduinoCloud.update();
  temperature = mockGetTemperature();
  waterLevel = mockGetWaterLvl();
  foodLevel = mockGetFoodLvl();
  powerConsumption = mockGetPowerConsumption();
  lightLevel = mockGetLightLevel();
  delay(5000);
  
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
