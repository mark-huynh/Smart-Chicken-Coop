/* Program: dimmer_test
 * Tests the heat lamp dimmer and malfunction detection
 * operations using test time values.
 * Author: Nicholas McBee
 * Created: 2/16/2021
 * Last Modified: 2/18/2021
 */
 
#define LAMP_MALFUNCTION_TRESH 1000 //Comparison value for ADC brightness measurement

const int dimmerPin = 3;       //Pin for PWM control of dimmer board
const int lampPhotoPin = A6;  //Photoresistor used for heat lamp
byte brightness_setting = 255; //Brightness setting for lamp
bool lamp_malfunction = 0;    //Indicates malfunction when high 
int currTime[3];              //Time array, holds dummy values for simulation


void setup() {
  Serial.begin(57600);
  currTime[0] = 0;
  currTime[1] = 0;
  currTime[2] = 0;
  while(currTime[0] < 21) { //Simulate time passing from midnight to 9pm
    lamp_controller();
    currTime[0]++;
    print_data();
    delay(500);
  }
  while(currTime[2] < 59) { //Simualtes 30 second dimming shutoff in real time
    lamp_controller();
    currTime[2]++;
    print_data();
    delay(1000);
  }

}

void loop() {

}

/* Func: print_data
 * Test bench function used to print the current test values
 * for time and heat lamp status
 */
void print_data() {
Serial.print("Time: ");
Serial.print(currTime[0]);
Serial.print(":");
Serial.print(currTime[1]);
Serial.print(":");
Serial.println(currTime[2]);
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
  if((currTime[0] >= 6) && (currTime[0] < 9)) { //Turns on lamp in early morning
    brightness_setting = 255;
  }
  else if((currTime[0] >= 18) && (currTime[0] < 21)) { //Turns on lamp in evening
    brightness_setting = 255;
  }
  else if(currTime[0] == 21) { //Dim lamp until off at nighttime
    dim_lamp();  
  }
  else {
    brightness_setting = 0;   //Turn off lamp at all other times
  }
  analogWrite(dimmerPin, brightness_setting); //Set signal for brightness
    
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
  int lamp_brightness = analogRead(lampThermistorPin);  
  if(lamp_brightness < LAMP_MALFUNCTION_TRESH) //Check if ADC reading is at corresponding light threshold
    return 1;
  return 0;
}
