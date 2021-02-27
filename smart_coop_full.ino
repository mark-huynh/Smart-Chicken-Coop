#include "thingProperties.h"
void setup() {
  //Setup for transmitter module
  Serial.begin(57600);
  delay(1500); // Search for serial monitor
  initProperties(); //Setup for transmitter

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

}

void loop() {

  //Update values and print to monitor
  getTime();
  temp = getTemp();
  lightLevel = getIfDaylight();
  foodLevel = getFoodLevel();
  printData();

  //Update cloud variables/graphs
  ArduinoCloud.update();
  temperature = GetTemp();
  waterLevel = mockGetWaterLvl();
  foodLevel = getFoodLevel();
  powerConsumption = mockGetPowerConsumption();
  lightLevel = getIfDaylight();

  lamp_controller(); //Handles heat lamp dimming and malfunction checking
  print_lamp_data();
  getTime();
  printData(); //Print values to serial monitor for testing

  doorController(); //Handles door opening and closing
  changeLights(waterLevel, foodLevel, temp, lamp_malfunction); //Handles warning light outputs
}
