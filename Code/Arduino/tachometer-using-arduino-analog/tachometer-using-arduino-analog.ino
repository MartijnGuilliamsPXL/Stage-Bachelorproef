#define IR_PIN A0 // Define the digital pin for the IR sensor
volatile unsigned long lastTime = 0; // Variable to store the last time a rotation was detected
volatile unsigned int rpm = 0; // Variable to store calculated RPM
unsigned long lastPrintTime = 0; // Variable to store the last time RPM was printed

void setup() {
  Serial.begin(2400); // Initialize serial communication
  pinMode(IR_PIN, INPUT); // Set the IR pin as input with internal pull-up resistor
  //attachInterrupt(digitalPinToInterrupt(IR_PIN), detectRotation, RISING); // Attach interrupt to the IR pin
}

void loop() {
  Serial.println(analogRead(IR_PIN));
}