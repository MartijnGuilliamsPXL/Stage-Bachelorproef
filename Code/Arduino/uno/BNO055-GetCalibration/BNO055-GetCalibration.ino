#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <EEPROM.h>

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup() {
  Serial.begin(115200);
  Serial.println("Orientation Sensor Calibration");

  if (!bno.begin())
  {
    Serial.println("Failed to initialize BNO055!");
    while (1);
  }

  // Calibrate the sensor
  Serial.println("Please perform sensor calibration...");
  delay(1000); // Delay to allow serial monitor to start

  while (!bno.isFullyCalibrated()) {
    uint8_t sys, gyro, accel, mag;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    Serial.print("System: ");
    Serial.print(sys, DEC);
    Serial.print(" Gyro: ");
    Serial.print(gyro, DEC);
    Serial.print(" Accel: ");
    Serial.print(accel, DEC);
    Serial.print(" Mag: ");
    Serial.println(mag, DEC);
    delay(500);
  }

  // Print calibration data
  adafruit_bno055_offsets_t calibrationData;
  bno.getSensorOffsets(calibrationData);
  Serial.println("Calibration Data:");
  Serial.print("Accelerometer: ");
  Serial.print(calibrationData.accel_offset_x);
  Serial.print(", ");
  Serial.print(calibrationData.accel_offset_y);
  Serial.print(", ");
  Serial.println(calibrationData.accel_offset_z);
  Serial.print("Gyroscope: ");
  Serial.print(calibrationData.gyro_offset_x);
  Serial.print(", ");
  Serial.print(calibrationData.gyro_offset_y);
  Serial.print(", ");
  Serial.println(calibrationData.gyro_offset_z);
  Serial.print("Magnetometer: ");
  Serial.print(calibrationData.mag_offset_x);
  Serial.print(", ");
  Serial.print(calibrationData.mag_offset_y);
  Serial.print(", ");
  Serial.println(calibrationData.mag_offset_z);
  Serial.print("Radius: ");
  Serial.println(calibrationData.accel_radius);

  // Hardcode calibration data into the program
  // Replace these values with the ones printed above
  bno.setSensorOffsets(calibrationData);
  displayCalStatus();
}

void loop() {
  
}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.println(mag, DEC);
}

