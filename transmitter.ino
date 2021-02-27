
float mockWaterLvl = 0;
float mockFoodLvl = 0;
float mockTemperature = 0;
bool mockLightLvl = false;
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

bool mockGetLightLevel() {
  mockLightLvl = !mockLightLvl;
  return mockLightLvl;
}

int mockGetPowerConsumption() {
  mockPowerLvl += 2;
  return mockPowerLvl;
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
