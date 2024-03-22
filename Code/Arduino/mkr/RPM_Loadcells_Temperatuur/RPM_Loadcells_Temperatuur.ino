// These define's must be placed at the beginning before #include "SAMDTimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select only one to be true for SAMD21. Must must be placed at the beginning before #include "SAMDTimerInterrupt.h"
#define USING_TIMER_TC3         true      // Only TC3 can be used for SAMD51
#define USING_TIMER_TC4         false     // Not to use with Servo library
#define USING_TIMER_TC5         false
#define USING_TIMER_TCC         false
#define USING_TIMER_TCC1        false
#define USING_TIMER_TCC2        true     // Don't use this, can crash on some boards

// Uncomment To test if conflict with Servo library
//#include "Servo.h"

/////////////////////////////////////////////////////////////////

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "SAMDTimerInterrupt.h"

//#ifndef LED_BUILTIN
//  #define LED_BUILTIN       13
//#endif

#ifndef LED_BLUE
  #define LED_BLUE          2
#endif

#ifndef LED_RED
  #define LED_RED           8
#endif


// TC3, TC4, TC5 max permissible TIMER_INTERVAL_MS is 1398.101 ms, larger will overflow, therefore not permitted
// Use TCC, TCC1, TCC2 for longer TIMER_INTERVAL_MS
// Init selected SAMD timer
SAMDTimer ITimer(TIMER_TC3);
SAMDTimer ITimer2(TIMER_TCC2);

#include "SAMDTimerInterrupt.h"
#define TIMER_INTERVAL_MS 100

volatile unsigned long lastTime;
volatile unsigned long currentTime;
volatile bool firstPulse = true;

volatile bool execute = false;
volatile unsigned long diffTime = 0;

volatile unsigned long rpm = 0;
volatile unsigned long count = 0;

#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define LOADCELL1_DOUT_PIN  1
#define LOADCELL1_SCK_PIN  2

#define LOADCELL2_DOUT_PIN  3
#define LOADCELL2_SCK_PIN  4

#define LOADCELL3_DOUT_PIN  5
#define LOADCELL3_SCK_PIN  6

#define LOADCELL4_DOUT_PIN  7
#define LOADCELL4_SCK_PIN  8

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

float weight_kg1 = 0;
float weight_kg2 = 0;
float weight_kg3 = 0;
float weight_kg4 = 0;

float weight_g1 = 0;
float weight_g2 = 0;
float weight_g3 = 0;
float weight_g4 = 0;

float Thrust_N = 0;

// Torque
float force1_newtons = 0;
float force2_newtons = 0;
float avgForce_newtons = 0;
float torque_Nm = 0;

float calibration_factor = -94850; //airpods(42g) -115050 //motor(195g) -78600 //lood(2883.3g)

#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>                  //Include the NonBlockingDallas library

#define ONE_WIRE_BUS 9                          //PIN of the Maxim DS18B20 temperature sensor
#define TIME_INTERVAL 1500                      //Time interval among sensor readings [milliseconds]

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas temperatureSensors(&dallasTemp);    //Create a new instance of the NonBlockingDallas class
double temperature = 0;

const int voltagePin = A1; // Analog pin for voltage output
const int currentPin = A2; // Analog pin for current output

#include <Servo.h>

Servo esc;  // create servo object to control the ESC

int throttlePin = 10;  // the pin where ESC is connected
int throttle = 1000;  // initial throttle value
int number = 0;
int digitCount = 0;

void setup() {
  Serial.begin(115200);
  //sensors.begin(); // Initialize the DallasTemperature library
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale3.begin(LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
  scale4.begin(LOADCELL4_DOUT_PIN, LOADCELL4_SCK_PIN);

  scale1.set_scale();
  scale1.tare();	//Reset the scale to 0
  scale2.set_scale();
  scale2.tare();	//Reset the scale to 0
  scale3.set_scale();
  scale3.tare();	//Reset the scale to 0
  scale4.set_scale();
  scale4.tare();	//Reset the scale to 0

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  long zero_factor3 = scale1.read_average(); //Get a baseline reading
  long zero_factor4 = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor1);


  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, rpm_measure, FALLING); 
  lastTime = 0;
  currentTime = 0;

  Serial.print(F("\nStarting ISR_RPM_Measure on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
  
  // Interval in millisecs
  if (ITimer.attachInterruptInterval_MS(TIMER_INTERVAL_MS, TimerHandler1))
  {
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  //Initialize the sensor passing the resolution and reading interval [milliseconds]
  temperatureSensors.begin(NonBlockingDallas::resolution_12, TIME_INTERVAL);

  //Callbacks
  temperatureSensors.onIntervalElapsed(handleIntervalElapsed);
 // temperatureSensors.onTemperatureChange(handleTemperatureChange);
  temperatureSensors.onDeviceDisconnected(handleDeviceDisconnected);
  
  //Call the following function whenever you want to request a new temperature reading without waiting for TIME_INTERVAL to elapse
  temperatureSensors.requestTemperature();

  esc.attach(throttlePin);  // attaches the ESC on pin 9 to the servo object
  esc.writeMicroseconds(2000); // Full speed
  delay(100);
  esc.writeMicroseconds(1000); // Off
  delay(100);  // send initial signal to keep ESC in idle state
  delay(3000);  // delay to allow the ESC to recognize the stopped signal
}

void loop() {
  temperatureSensors.update();
  if(execute)
  {
    currentTime = micros();
    diffTime = currentTime - lastTime;
    
    if(diffTime > 1000000)
    {
      rpm = 0;
    }
    Serial.print("RPM:");
    Serial.print(rpm);
    Serial.print(",");
    //Serial.println(rpm);
    //Serial.println(count);

    Serial.print("temperature:");
    Serial.print(temperature);
    Serial.print(",");
    
    scale1.set_scale(calibration_factor); //Adjust to this calibration factor
    scale2.set_scale(calibration_factor); //Adjust to this calibration factor
    scale3.set_scale(calibration_factor); //Adjust to this calibration factor
    scale4.set_scale(calibration_factor); //Adjust to this calibration factor

    weight_kg1 = scale1.get_units() * 0.453592; // Convert lbs to Kg
    weight_kg2 = scale2.get_units() * 0.453592; // Convert lbs to Kg
    weight_kg3 = scale3.get_units() * 0.453592; // Convert lbs to Kg
    weight_kg4 = scale4.get_units() * 0.453592; // Convert lbs to Kg

    weight_g1 = weight_kg1 * 1000; // Convert Kg to g
    weight_g2 = weight_kg2 * 1000; // Convert Kg to g
    weight_g3 = weight_kg3 * 1000; // Convert Kg to g
    weight_g4 = weight_kg4 * 1000; // Convert Kg to g

    // Thrust
    Thrust_N = (weight_kg1 + weight_kg2) * 9.81;

    // Torque
    force1_newtons = weight_kg3 * (9.81);
    force2_newtons = weight_kg4 * (9.81);
    avgForce_newtons = (force1_newtons - force2_newtons) / 2;
    torque_Nm = avgForce_newtons * 0.03365;

    Serial.print("Thrust N:");
    Serial.print(Thrust_N, 2);
    Serial.print(",");
    Serial.print("Torque Nm:");
    Serial.print(torque_Nm,6);
    Serial.print(",");

    int voltageRaw = analogRead(voltagePin);
    int currentRaw = analogRead(currentPin);
    // Print values to serial monitor
    Serial.print("Voltage (V):");
    Serial.print(voltageRaw * 0.0503861788617886); // Display with 2 decimal places * 0.0503861788617886
    Serial.print(",");
    Serial.print("Current (A):");
    Serial.print(currentRaw * 0.0571428571428571); // Display with 2 decimal places * 0.0571428571428571

    Serial.println();
    execute = false;
  }

  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    // Check if character is a valid digit (0-9)
    if (incomingByte >= '0' && incomingByte <= '9') {
      // Convert ASCII digit to integer (subtract '0' from ASCII value)
      int digit = incomingByte - '0';
      
      // Check if exceeding 4 digits
      if (digitCount < 4) {
        // Shift existing number left by 1 digit (multiply by 10)
        number *= 10;
        // Add new digit to the end
        number += digit;
        digitCount++;
      }
    } else if (incomingByte == '\n' && digitCount == 4) {
      // Newline received after 4 digits, data complete
      // Process the received number (stored in 'number')
      //Serial.println(number);
      if((number >=1000) && (number <=2000))
      {
        throttle = number;
      }
      number = 0;
      digitCount = 0;
    } else {
      // Reset if invalid character or exceeding 4 digits without newline
      number = 0;
      digitCount = 0;
    }
  }

  //Serial.println(throttle);
  esc.writeMicroseconds(throttle);
}

void rpm_measure() {
  if (firstPulse) {
    lastTime = micros();
    firstPulse = false;
  } else {
    currentTime = micros();
    diffTime = currentTime - lastTime;
    rpm = 60.0 / (diffTime / 1000000.0);
    //Serial.print("RPM = ");
    //Serial.println(diffTime);
    lastTime = currentTime;
  }
}

void TimerHandler1(void)
{
  execute = true;
  count++;
  //Serial.println("test");
}

//Invoked at every VALID sensor reading. "valid" parameter will be removed in a feature version
void handleIntervalElapsed(int deviceIndex, int32_t temperatureRAW)
{
  temperature = temperatureSensors.rawToCelsius(temperatureRAW);
}

//Invoked when the sensor reading fails
void handleDeviceDisconnected(int deviceIndex)
{
  Serial.print(F("[NonBlockingDallas] handleDeviceDisconnected ==> deviceIndex="));
  Serial.print(deviceIndex);
  Serial.println(F(" disconnected."));
}
