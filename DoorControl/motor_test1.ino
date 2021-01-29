//Michael Crockett
//Door_operation_test
//1/29/2021
 
 
#define INPUT1     9
#define INPUT2     8
#define ENABLE  10
 
#define Low 75 // determined as the optimal speed after testing
 
int Speed; 
 
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
 
void setup() {
  
  Serial.begin(9600);
 
  pinMode(ENABLE, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
}
 
void loop() {
  Speed=Low; // Testing showed that 75 was about as slow as it could be set while still maintaining reliable operation.
  opening();
  delay(5000);
  closing();
  delay(5000);
}