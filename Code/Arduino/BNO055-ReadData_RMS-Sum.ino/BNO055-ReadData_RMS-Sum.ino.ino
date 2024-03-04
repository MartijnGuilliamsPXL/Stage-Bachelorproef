#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <arduinoFFT.h>
#include <math.h> //Include math library

#define SAMPLING_FREQUENCY 1000 //Hz, must be at least twice the maximum frequency to measure
#define SAMPLES 32 //Must be a power of 2

arduinoFFT FFT = arduinoFFT();

double vRealX[SAMPLES];
double vImagX[SAMPLES];
double vRealY[SAMPLES];
double vImagY[SAMPLES];
double vRealZ[SAMPLES];
double vImagZ[SAMPLES];

double x = -1000000, y = -1000000 , z = -1000000; //dumb values, easy to spot problem

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

  unsigned long microseconds;
  //Serial.print("OK1");
  /*SAMPLING*/
  for (int i = 0; i < SAMPLES; i++) {
    bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    printEvent(&accelerometerData); 
    //int max_value = max(x, max(y, z)); 

    microseconds = micros();
    vRealX[i] = x; //Read the x-axis acceleration
    vImagX[i] = 0;
    vRealY[i] = y; //Read the y-axis acceleration
    vImagY[i] = 0;
    vRealZ[i] = z; //Read the z-axis acceleration
    vImagZ[i] = 0;
    while (micros() < (microseconds + (1000000 / SAMPLING_FREQUENCY)));
  }

  /*FFT*/
  FFT.Windowing(vRealX, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vRealX, vImagX, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vRealX, vImagX, SAMPLES);
  double peakX = FFT.MajorPeak(vRealX, SAMPLES, SAMPLING_FREQUENCY);

  FFT.Windowing(vRealY, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vRealY, vImagY, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vRealY, vImagY, SAMPLES);
  double peakY = FFT.MajorPeak(vRealY, SAMPLES, SAMPLING_FREQUENCY);

  FFT.Windowing(vRealZ, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vRealZ, vImagZ, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vRealZ, vImagZ, SAMPLES);
  double peakZ = FFT.MajorPeak(vRealZ, SAMPLES, SAMPLING_FREQUENCY);

  /*RMS*/
  //Assuming you have already defined and initialized the variables and arrays as in your previous code
  double rmsSum; //Declare a variable to store the RMS sum
  double sum = 0; //Declare a variable to store the sum of squares
  for (int i = 0; i < SAMPLES / 2; i++) { //Loop through the frequency bins
    sum += vRealX[i] * vRealX[i] + vRealY[i] * vRealY[i] + vRealZ[i] * vRealZ[i]; //Add the squares of the amplitudes along all three axes
  }
  rmsSum = sqrt(sum / (SAMPLES / 2)); //Divide the sum of squares by the number of bins and take the square root
  Serial.print("RMS Sum:");
  Serial.println(rmsSum); //Print the RMS sum value
}

void printEvent(sensors_event_t* event) {
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
    /*
    Serial.print("acceleration.x:");
    Serial.print(x);
    Serial.print(",");
    Serial.print("acceleration.y:");
    Serial.print(y);
    Serial.print(",");
    Serial.print("acceleration.z:");
    Serial.print(z);
    */
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
