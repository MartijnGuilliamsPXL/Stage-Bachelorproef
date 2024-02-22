// Rotary Encoder RPM Measurement using Timer Interrupts

const int sensorPin = 2; // Example pin for sensor input

volatile unsigned int counts = 0; // Counter for rotations
volatile unsigned int rpm = 0; // Variable to store RPM
volatile unsigned long prevTime = 0; // Variable to store previous time

volatile bool execute = false;

void setup() {
  Serial.begin(115200);
  
  // Attach interrupt to sensor pin
  attachInterrupt(digitalPinToInterrupt(sensorPin), countRotations, RISING);

  // Pause interrupts
  cli();

  // Reset counter1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  // Set the value that the counter has to reach before it triggers an interrupt
  // 15624 = 1s (assuming you use 1024 as the prescaler value)
  // Counter1 is 16-bit so the value must be less than or equal to 65535
  OCR1A = 15624;
 
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
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - prevTime;
  prevTime = currentTime;

  // Calculate RPM
  rpm = (counts * 60000) / elapsedTime; // Convert from pulses per second to pulses per minute


  // Print RPM to serial monitor
  //Serial.print("RPM: ");
  //Serial.println(rpm);

  // Reset rotation counter
  //counts = 0;

	// Don't forget to reset the flag
	execute = false;
  }
}

void countRotations() {
  counts++; // Increment rotation counter
  Serial.println(counts);
}

ISR(TIMER1_COMPA_vect)
{
  // The MCU calls this function whenever the timer resets itself
  // (i.e., once every second).
 
  execute = true;
}

