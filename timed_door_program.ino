//Michael Crockett
//timed_door_operation_test
//1/29/2021
 
#include "RTClib.h" 
DS3231 rtc;
#define INPUT1     9
#define INPUT2     8
#define ENABLE  10
 
#define Low 75 // determined as the optimal speed after testing
 
int Speed; 
int currTime[3];
 
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
 
void getTime() {
  DateTime now = rtc.now();
  
  currTime[0] = now.hour();
  currTime[1] = now.minute();
  currTime[2] = now.second();
  
}

void setup() {
  
  Serial.begin(9600);
 
  pinMode(ENABLE, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
}
 
void loop() {
  getTime();
  Speed=Low; // Testing showed that 75 was about as slow as it could be set while still maintaining reliable operation.
    if((currTime[0] = 7)&& (currTime[1] = 0)&&(currTime[0] = 0)) {
      opening();
    }
    if((currTime[0] = 5)&& (currTime[1] = 0)&&(currTime[0] = 0)) {
    closing();
    }
  
}