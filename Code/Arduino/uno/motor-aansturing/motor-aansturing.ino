#include <Servo.h>

Servo esc;  // create servo object to control the ESC

int throttlePin = 10;  // the pin where ESC is connected
int throttle = 1000;  // initial throttle value
int number = 0;
int digitCount = 0;

void setup() {
  esc.attach(throttlePin);  // attaches the ESC on pin 9 to the servo object
  esc.writeMicroseconds(2000); // Full speed
  delay(100);
  esc.writeMicroseconds(1000); // Off
  delay(100);  // send initial signal to keep ESC in idle state
  delay(3000);  // delay to allow the ESC to recognize the stopped signal

  Serial.begin(115200);  // start serial communication at 9600bps
}

void loop() {
  char input = "";
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

  Serial.println(throttle);
  esc.writeMicroseconds(throttle);  // control the ESC depending on throttle input
  //delay(20);  // delay to ensure consistent signal timing
}