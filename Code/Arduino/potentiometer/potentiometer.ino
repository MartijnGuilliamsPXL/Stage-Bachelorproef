int potPin = A5; // Potentiometer output connected to analog pin 3
int potVal = 0; // Variable to store the input from the potentiometer

void setup()
{
  Serial.begin(115200);
  pinMode(10, OUTPUT);
}

// Main program
void loop()
{
  potVal = analogRead(potPin);   // read the potentiometer value at the input pin

  Serial.println(potVal/4);

  analogWrite(10, (potVal/4)-1);

}