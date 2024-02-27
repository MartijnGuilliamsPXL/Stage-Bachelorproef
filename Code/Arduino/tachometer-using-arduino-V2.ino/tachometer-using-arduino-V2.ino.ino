volatile unsigned long lastTime;
volatile unsigned long currentTime;
volatile bool firstPulse = true;

volatile bool execute = false;
volatile unsigned long diffTime = 0;

volatile unsigned long rpm = 0;

void setup() {
  Serial.begin(115200);
  attachInterrupt(0, rpm_measure, FALLING); // interrupt 0 is on pin 2
  lastTime = 0;
  currentTime = 0;

  // Pause interrupts
  cli();

  // Reset counter1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  // Set the value that the counter has to reach before it triggers an interrupt
  // 15624 = 1s (assuming you use 1024 as the prescaler value)
  // Counter1 is 16-bit so the value must be less than or equal to 65535
  OCR1A = 156.24; //3906 1/4s //15624 =1s
 
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
    currentTime = micros();
    diffTime = currentTime - lastTime;
    
    if(diffTime > 1000000)
    {
      rpm = 0;
    }
    
    Serial.println(rpm/2);
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
  }
}

ISR(TIMER1_COMPA_vect)
{
  // The MCU calls this function whenever the timer resets itself
  // (i.e., once every second).
 
  execute = true;
}