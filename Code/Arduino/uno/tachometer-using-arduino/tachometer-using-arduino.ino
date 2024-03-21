const int sensorPin = 2; // Example pin for sensor input

volatile unsigned int counts = 0; // Counter for rotations
volatile unsigned int rpm = 0; // Variable to store RPM
volatile unsigned long prevTime = 0; // Variable to store previous time

volatile bool execute = false;
volatile int lastSensorState = LOW; // Variable to store the previous state of the sensor

#define WINDOW_SIZE 20

int INDEX = 0;
int VALUE = 0;
volatile unsigned long SUM = 0;
int READINGS[WINDOW_SIZE];
volatile unsigned int AVERAGED = 0;

void setup() {
  Serial.begin(115200);
  
  // Attach interrupt to sensor pin
  attachInterrupt(digitalPinToInterrupt(sensorPin), countRotations, FALLING);

  // Pause interrupts
  cli();

  // Reset counter1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  // Set the value that the counter has to reach before it triggers an interrupt
  // 15624 = 1s (assuming you use 1024 as the prescaler value)
  // Counter1 is 16-bit so the value must be less than or equal to 65535
  OCR1A = 1024; //3906 1/4s //15624 =1s
 
  // Clear timer on compare match
  // The timer resets itself when it reaches 15625 (OCR1A +1)
  TCCR1B |= (1 << WGM12);
 
  // Set the prescaler to 1024 (See ATMega328PU datasheet for infos)
  TCCR1B |= (1 << CS12) | (1 << CS10);
 
  // Enable timer interrupt
  TIMSK1 |= (1 << OCIE1A);

  // Enable interrupts
  sei();
}

void loop() {
  if(execute)
  {
	// Calculate RPM
  unsigned long currentTime = micros();
  unsigned long elapsedTime = currentTime - prevTime;
  prevTime = currentTime;

  // Calculate RPM
  rpm = (counts * 60000) / elapsedTime; // Convert from pulses per second to pulses per minute


  // Print RPM to serial monitor
  //Serial.print("RPM: ");
  //Serial.println(rpm);


  SUM = SUM - READINGS[INDEX];       // Remove the oldest entry from the sum 
  READINGS[INDEX] = rpm;           // Add the newest reading to the window
  SUM = SUM + rpm;                 // Add the newest reading to the sum
  INDEX = (INDEX+1) % WINDOW_SIZE;   // Increment the index, and wrap to 0 if it exceeds the window size

  AVERAGED = SUM / WINDOW_SIZE;      // Divide the sum of the window by the window size for the result

  Serial.println(AVERAGED);
  //Serial.print(",");
  //Serial.println(AVERAGED);




  // Reset rotation counter
  //Serial.println(counts);
  counts = 0;

	// Don't forget to reset the flag
	execute = false;
  }
}

void countRotations() {
  static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  unsigned long debounceDelay = 1;    // the debounce time; increase if the output flickers

  // Read the state of the sensor


  int currentState = digitalRead(sensorPin);

  // If the state has changed since the last read, debounce
  if (currentState != lastSensorState) {
    lastDebounceTime = micros();
  }

  // Check if a new rotation is detected after debouncing
  if ((micros() - lastDebounceTime) > debounceDelay) {
    counts++; // Increment rotation counter
  }

  lastSensorState = currentState;
}

ISR(TIMER1_COMPA_vect)
{
  // The MCU calls this function whenever the timer resets itself
  // (i.e., once every second).
 
  execute = true;
}
