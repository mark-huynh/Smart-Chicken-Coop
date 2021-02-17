const int RED_PIN = D4;  // the Arduino pin, which connects to the IN pin of relay
const int YELLOW_PIN = D3;  // the Arduino pin, which connects to the IN pin of relay
const int GREEN_PIN = D2;  // the Arduino pin, which connects to the IN pin of relay
const int ALARM_PIN = D1;  // the Arduino pin, which connects to the IN pin of relay


void changeLights(float waterLvl, float foodLvl, float temperature, int powerConsumption) {
  int yellow_light = waterLvl < 20 || foodLvl < 20;
  int red_light = temperature <= 34;
  int blue_light = powerConsumption == 0;
  int green_light = !(yellow_light || red_light || blue_light);
  digitalWrite(RED_PIN, red_light);
  digitalWrite(YELLOW_PIN, yellow_light);
  digitalWrite(GREEN_PIN, green_light);
  digitalWrite(ALARM_PIN, blue_light);
}

void returnNormalLights() {
  changeLights(100, 100, 100, 100);
  delay(1000);
}

void setup() {
  // initialize digital relay pin as an output.
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);
}

void loop() {
  Serial.println("Mocking full food, full water, properly working heat lamp, and non-freezing temperatures (Green light should illuminate)");
  changeLights(100, 100, 100, 100);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing temperature warning, mocking temperature of 34C (Red light should illuminate)");
  changeLights(100, 100, 34, 100);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing food warning, mocking food level of 1 gram (Yellow light should illuminate)");
  changeLights(100, 1, 100, 100);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing water warning, mocking water level of 1 mL (Yellow light should illuminate)");
  changeLights(1, 100, 100, 100);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing malfunctioning heat lamp, mocking power consumption of 0W (Blue light [alarm] should illuminate)");
  changeLights(100, 100, 100, 0);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing low resources and temperature warning (Red + Yellow)");
  changeLights(1, 100, 32, 100);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing low resources, temperature warning, and heat lamp malfunction (all lights but green)");
  changeLights(1, 1, 1, 0);
  delay(3500);
  returnNormalLights();
}