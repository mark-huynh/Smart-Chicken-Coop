#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char THING_ID[]           = "0af5ae20-c645-4643-886e-f1717df3f241";
const char DEVICE_LOGIN_NAME[]  = "c4c799ba-626a-4ea9-abec-8f700d360e13";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onTemperatureChange();
void onWaterLevelChange();
void onFoodLevelChange();
void onPowerConsumptionChange();
void onLightLevelChange();

CloudTemperature temperature;
CloudVolume waterLevel;
CloudMass foodLevel;
int powerConsumption;
int lightLevel;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, onTemperatureChange);
  ArduinoCloud.addProperty(waterLevel, READWRITE, ON_CHANGE, onWaterLevelChange);
  ArduinoCloud.addProperty(foodLevel, READWRITE, ON_CHANGE, onFoodLevelChange);
  ArduinoCloud.addProperty(powerConsumption, READWRITE, ON_CHANGE, onPowerConsumptionChange);
  ArduinoCloud.addProperty(lightLevel, READWRITE, ON_CHANGE, onLightLevelChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
