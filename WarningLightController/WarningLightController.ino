const int RELAY_PIN = 3;  // the Arduino pin, which connects to the IN pin of relay

float mockWaterLvl = 0;
float mockFoodLvl = 0;
float mockTemperature = 0;

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

void setup() {
  // initialize digital relay pin as an output.
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
//   digitalWrite(RELAY_PIN, HIGH);
//   delay(500);
//   digitalWrite(RELAY_PIN, LOW);
//   delay(500);
}