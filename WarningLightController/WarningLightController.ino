const int RED_PIN = D4;  // the Arduino pin, which connects to the IN pin of relay for the RED LED
const int YELLOW_PIN = D3;  // the Arduino pin, which connects to the IN pin of relay for the YELLOW LED
const int GREEN_PIN = D2;  // the Arduino pin, which connects to the IN pin of relay for the GREEN LED
const int ALARM_PIN = D1;  // the Arduino pin, which connects to the IN pin of relay for the BLUE LED


// Function that will take each of the sensor values and determine corresponding LEDs to turn on
void changeLights(float waterLvl, float foodLvl, float temperature, bool lampMalfunction) {
  int yellow_light = waterLvl < 100 || foodLvl < 100; //Resources threshold
  int red_light = temperature <= 34; // Temperature threshold
  int blue_light = lampMalfunction; // If lamp is malfunctioning
  int green_light = !(yellow_light || red_light || blue_light); // If no other lights are on, green should be on

//   Write all of the determined values to corresponding digital pins
  digitalWrite(RED_PIN, red_light);
  digitalWrite(YELLOW_PIN, yellow_light);
  digitalWrite(GREEN_PIN, green_light);
  digitalWrite(ALARM_PIN, blue_light);
}

// Function to return lights back to green status
void returnNormalLights() {
  changeLights(100, 100, 100, 100);
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  // initialize all digital relay pins as an output.
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);
}

void loop() {
  Serial.println("Mocking full food, full water, properly working heat lamp, and non-freezing temperatures (Green light should illuminate)");
  changeLights(100, 100, 100, false);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing temperature warning, mocking temperature of 34C (Red light should illuminate)");
  changeLights(100, 100, 34, false);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing food warning, mocking food level of 1 gram (Yellow light should illuminate)");
  changeLights(100, 1, 100, false);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing water warning, mocking water level of 1 mL (Yellow light should illuminate)");
  changeLights(1, 100, 100, false);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing malfunctioning heat lamp (Blue light [alarm] should illuminate)");
  changeLights(100, 100, 100, true);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing low resources and temperature warning (Red + Yellow)");
  changeLights(1, 100, 32, false);
  delay(3500);
  returnNormalLights();

  Serial.println("Testing low resources, temperature warning, and heat lamp malfunction (all lights but green)");
  changeLights(1, 1, 1, true);
  delay(3500);
  returnNormalLights();
}