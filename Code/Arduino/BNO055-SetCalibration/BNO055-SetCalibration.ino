#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <EEPROM.h>

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

  Serial.println("Calibration data set successfully.");
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
}

void loop() {
  // Your loop code here
}
