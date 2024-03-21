const int voltagePin = A1; // Analog pin for voltage output
const int currentPin = A0; // Analog pin for current output

int potPinIN = A5;
int potPinOUT = 10;
int potVal = 0; // Variable to store the input from the potentiometer

const int numSamples = 20; // Number of samples for moving average
float voltageSamples[numSamples];
float currentSamples[numSamples];
int sampleIndex = 0;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numSamples; i++) {
    voltageSamples[i] = 0;
    currentSamples[i] = 0;
  }

  pinMode(potPinOUT, OUTPUT);
}

void loop() {
  // Read analog values
  int voltageRaw = analogRead(voltagePin);
  int currentRaw = analogRead(currentPin);

  // Update moving average arrays
  voltageSamples[sampleIndex] = voltageRaw * 0.070769230769;
  currentSamples[sampleIndex] = currentRaw * 0.1461111;
  sampleIndex = (sampleIndex + 1) % numSamples;

  // Calculate moving averages
  float avgVoltage = 0;
  float avgCurrent = 0;
  for (int i = 0; i < numSamples; i++) {
    avgVoltage += voltageSamples[i];
    avgCurrent += currentSamples[i];
  }
  avgVoltage /= numSamples;
  avgCurrent /= numSamples;

  // Print moving average values to serial monitor
  Serial.print("Moving Avg Voltage (V):");
  Serial.print(avgVoltage, 2); // Display with 2 decimal places
  Serial.print(",");
  Serial.print("Moving Avg Current (A):");
  Serial.println(avgCurrent, 2); // Display with 2 decimal places

  potVal = analogRead(potPinIN);   // read the potentiometer value at the input pin
  //Serial.println(potVal/4);
  analogWrite(potPinOUT, (potVal/4)-1);
}