#include <Servo.h>

Servo esc;  // create servo object to control the ESC

int throttlePin = 9;  // the pin where ESC is connected
int throttle = 1000;  // initial throttle value

void setup() {
  esc.attach(throttlePin);  // attaches the ESC on pin 9 to the servo object
  esc.writeMicroseconds(2000); // Full speed
  delay(100);
  esc.writeMicroseconds(1000); // Off
  delay(100);  // send initial signal to keep ESC in idle state
  delay(3000);  // delay to allow the ESC to recognize the stopped signal

  Serial.begin(9600);  // start serial communication at 9600bps
}

void loop() {
  if (Serial.available() > 0) {
    int temp = Serial.parseInt();
    if (temp >= 1000){
      throttle = temp;  // read the incoming number
      //Serial.println(throttle);
      throttle = constrain(throttle, 1000, 2000);  // constrain the throttle signal to 1000-2000
    }
    
  }
  Serial.println(throttle);
  esc.writeMicroseconds(throttle);  // control the ESC depending on throttle input
  delay(20);  // delay to ensure consistent signal timing
}
