/* Program: dimmer_test
 * Tests the heat lamp dimmer and malfunction detection
 * Author: Nicholas McBee
 * Created: 2/16/2021
 * Last Modified: 2/26/2021
 */
 
#define LAMP_MALFUNCTION_TRESH 1000 //Comparison value for ADC brightness measurement
#define LAMP_ON_AM 6                // Time that lamp will turn on in morning
#define LAMP_OFF_AM 9               // Time lamp will turn off in morning
#define LAMP_ON_PM 18               // Time lamp will turn on at night
#define LAMP_OFF_PM 21              // Time lamp will turn off at night
#define LAMP_WATTAGE 4              //Wattage rating of lamp bulb

const int dimmerPin = 3;       //Pin for PWM control of dimmer board
const int lampPhotoPin = A6;  //Photoresistor used for heat lamp

byte brightness_setting = 0; //Brightness setting for lamp
bool lamp_malfunction = 0;    //Indicates malfunction when high 
int currTime[3];              //Time array, holds dummy values for simulation
float power_consumption = 0;    //Power consumed by lamp in Watt-hours

/* Func: print_lamp_data
 * Test bench function used to print the current test values
 * for time and heat lamp status
 */
void print_lamp_data() {
Serial.print("Power Consumption: ");
Serial.println(power_consumption);
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
  if((currTime[0] >= LAMP_ON_AM) && (currTime[0] < LAMP_OFF_AM)) { //Turns on lamp in early morning
    brightness_setting = 255;
  }
  else if((currTime[0] >= LAMP_ON_PM) && (currTime[0] < LAMP_OFF_PM)) { //Turns on lamp in evening
    brightness_setting = 255;
  }
  else if(currTime[0] == LAMP_OFF_PM) { //Dim lamp until off at nighttime
    dim_lamp();  
  }
  else {
    brightness_setting = 0;   //Turn off lamp at all other times
  }
  analogWrite(dimmerPin, brightness_setting); //Set signal for brightness
    
}

/* Function: get_power_consumption
 * Calculates the power consumption of the heat lamp for the current day.
 */
float get_power_consumption() {
  float lamp_time = (LAMP_OFF_AM - LAMP_ON_AM) + (LAMP_OFF_PM - LAMP_ON_PM); //Time lamp is turned on
  float power = LAMP_WATTAGE*lamp_time; //Compute watt-hour usage
  return power;
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
  int lamp_brightness = analogRead(lampPhotoPin);  
  if(lamp_brightness < LAMP_MALFUNCTION_TRESH) //Check if ADC reading is at corresponding light threshold
    return 1;
  return 0;
}
