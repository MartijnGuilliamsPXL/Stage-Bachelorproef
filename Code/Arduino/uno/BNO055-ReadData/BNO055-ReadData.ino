#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup() {
  Serial.begin(115200);
  Serial.println("Setting Calibration Data");

  if (!bno.begin())
  {
    Serial.println("Failed to initialize BNO055!");
    while (1);
  }

  // Hardcode calibration data into the program
  adafruit_bno055_offsets_t calibrationData;

  // Set the calibration data to the values you copied and saved elsewhere
  calibrationData.accel_offset_x = 5;
  calibrationData.accel_offset_y = 44;
  calibrationData.accel_offset_z = -26;
  calibrationData.gyro_offset_x = -2;
  calibrationData.gyro_offset_y = -2;
  calibrationData.gyro_offset_z = 1;
  calibrationData.mag_offset_x = 961;
  calibrationData.mag_offset_y = -68;
  calibrationData.mag_offset_z = -99;
  calibrationData.accel_radius = 1000;

  bno.setSensorOffsets(calibrationData);

  delay(1000);
}

void loop(void)
{
  //could add VECTOR_ACCELEROMETER, VECTOR_MAGNETOMETER,VECTOR_GRAVITY...
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  //Serial.print(",");
  //printEvent(&orientationData); //orientation
  //Serial.print(",");
  //printEvent(&angVelocityData); //GYRO
  //Serial.print(",");
  //printEvent(&linearAccelData); //acceleration2
  //Serial.print(",");
  //printEvent(&magnetometerData); //magnetic
  //Serial.print(",");
  printEvent(&accelerometerData); //acceleration
  //Serial.print(",");
  //printEvent(&gravityData); //acceleration3
  //Serial.print(",");

  int8_t boardTemp = bno.getTemp();
  Serial.println();
  //Serial.print(F("temperature: "));
  //Serial.println(boardTemp);

  delay(1000);
}

void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    Serial.print("acceleration.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("acceleration.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("acceleration.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
    Serial.print("orientation.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("orientation.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("orientation.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
    Serial.print("magnetic.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("magnetic.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("magnetic.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
    Serial.print("gyro.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("gyro.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("gyro.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
    Serial.print("gyro2.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("gyro2.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("gyro2.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    Serial.print("acceleration2.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("acceleration2.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("acceleration2.z:");
    Serial.print(z);
  }
  else if (event->type == SENSOR_TYPE_GRAVITY) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    Serial.print("acceleration3.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("acceleration3.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("acceleration3.z:");
    Serial.print(z);
  }
  else {
    // do nothing
  }

  // print the values separated by commas
  /*
  Serial.print("Variable_1:");
  Serial.print(x);
  Serial.print(",");
  Serial.print("Variable_2:");
  Serial.print(y);
  Serial.print(",");
  Serial.print("Variable_3:");
  Serial.print(z);
  */
}
