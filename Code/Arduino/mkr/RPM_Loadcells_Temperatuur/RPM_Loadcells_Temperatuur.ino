#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
      || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
      || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) \
      || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD51__) || defined(__SAMD51J20A__) \
      || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__)  \
      || defined(__SAMD21E15A__) || defined(__SAMD21E16A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__) \
      || defined(__SAMD21G15A__) || defined(__SAMD21G16A__) || defined(__SAMD21G17A__) || defined(__SAMD21G18A__) \
      || defined(__SAMD21J15A__) || defined(__SAMD21J16A__) || defined(__SAMD21J17A__) || defined(__SAMD21J18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

/////////////////////////////////////////////////////////////////

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
#define USING_TIMER_TCC2        false     // Don't use this, can crash on some boards

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
#define TIMER_INTERVAL_MS          1
#define DEBOUNCING_INTERVAL_MS      80

#define LOCAL_DEBUG                 1

///////////////////////////////////////////////

#if (TIMER_INTERRUPT_USING_SAMD21)

  #if USING_TIMER_TC3
    #define SELECTED_TIMER      TIMER_TC3
  #elif USING_TIMER_TC4
    #define SELECTED_TIMER      TIMER_TC4
  #elif USING_TIMER_TC5
    #define SELECTED_TIMER      TIMER_TC5
  #elif USING_TIMER_TCC
    #define SELECTED_TIMER      TIMER_TCC
  #elif USING_TIMER_TCC1
    #define SELECTED_TIMER      TIMER_TCC1
  #elif USING_TIMER_TCC2
    #define SELECTED_TIMER      TIMER_TCC
  #else
    #error You have to select 1 Timer  
  #endif

#else

  #if !(USING_TIMER_TC3)
    #error You must select TC3 for SAMD51
  #endif
  
  #define SELECTED_TIMER      TIMER_TC3

#endif  

// Init selected SAMD timer
SAMDTimer ITimer(SELECTED_TIMER);

////////////////////////////////////////////////
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

float calibration_factor = -94850; //airpods(42g) -115050 //motor(195g) -78600 //lood(2883.3g)

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 9 // Define the pin for the OneWire data

OneWire oneWire(ONE_WIRE_BUS); // Create a OneWire object
DallasTemperature sensors(&oneWire); // Create a DallasTemperature object

void setup() {
  Serial.begin(115200);
  sensors.begin(); // Initialize the DallasTemperature library
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
  if (ITimer.attachInterruptInterval_MS(TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

void loop() {
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
    count = 0;

    sensors.requestTemperatures(); // Request temperature measurements from the sensor
    float temperature = sensors.getTempCByIndex(0); // Get the temperature measurement in Celsius
    Serial.print("temperature:");
    Serial.print(temperature);
    Serial.print(",");

    scale1.set_scale(calibration_factor); //Adjust to this calibration factor
    scale2.set_scale(calibration_factor); //Adjust to this calibration factor
    scale3.set_scale(calibration_factor); //Adjust to this calibration factor
    scale4.set_scale(calibration_factor); //Adjust to this calibration factor

    float weight_kg1 = scale1.get_units() * 0.453592; // Convert lbs to Kg
    float weight_kg2 = scale2.get_units() * 0.453592; // Convert lbs to Kg
    float weight_kg3 = scale3.get_units() * 0.453592; // Convert lbs to Kg
    float weight_kg4 = scale4.get_units() * 0.453592; // Convert lbs to Kg

    float weight_g1 = weight_kg1 * 1000; // Convert Kg to g
    float weight_g2 = weight_kg2 * 1000; // Convert Kg to g
    float weight_g3 = weight_kg3 * 1000; // Convert Kg to g
    float weight_g4 = weight_kg4 * 1000; // Convert Kg to g

    // Thrust
    float Thrust_N = (weight_kg1 + weight_kg2) * 9.81;

    // Torque
    float force1_newtons = weight_kg3 * (9.81);
    float force2_newtons = weight_kg4 * (9.81);
    float avgForce_newtons = (force1_newtons - force2_newtons) / 2;
    float torque_Nm = avgForce_newtons * 0.03365;

    Serial.print("Thrust N:");
    Serial.print(Thrust_N, 2);
    Serial.print(",");
    Serial.print("Torque Nm:");
    Serial.print(torque_Nm,6);



    Serial.println();
    execute = false;
  }
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
    count++;
  }
}

void TimerHandler(void)
{
  execute = true;
  //Serial.println("test");
}
