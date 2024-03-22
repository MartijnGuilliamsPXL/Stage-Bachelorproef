// SEN-16408 Current and Voltage Sensor Example
// Read voltage and current values from the sensor

const int voltagePin = A1; // Analog pin for voltage output
const int currentPin = A2; // Analog pin for current output

int potPinIN = A5;
int potPinOUT = 10;
int potVal = 0; // Variable to store the input from the potentiometer

void setup() {
  Serial.begin(115200);

  pinMode(potPinOUT, OUTPUT);
}

void loop() {
  // Read analog values
  int voltageRaw = analogRead(voltagePin);
  int currentRaw = analogRead(currentPin);

  // Print values to serial monitor
  Serial.print("Voltage (V):");
  Serial.print(voltageRaw * 0.0503861788617886); // Display with 2 decimal places * 0.0503861788617886
  Serial.print(",");
  Serial.print("Current (A):");
  Serial.println(currentRaw * 0.0571428571428571); // Display with 2 decimal places * 0.0571428571428571

  
  potVal = analogRead(potPinIN);   // read the potentiometer value at the input pin
  //Serial.println(potVal/4);
  analogWrite(potPinOUT, (potVal/4)-1);
  
  //delay(10); // Wait for 1 second
}