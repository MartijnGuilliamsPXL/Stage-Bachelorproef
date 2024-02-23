#include <arduinoFFT.h>

#define SAMPLES 64             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Max 10,000 for Arduino Uno

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];

arduinoFFT FFT = arduinoFFT();

void setup() {
  Serial.begin(9600);
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

void loop() {

  /*Collect and store data*/
  microseconds = micros();
  for(int i=0; i<SAMPLES; i++) {
      vReal[i] = analogRead(A0);
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){
        //empty loop
      }
      microseconds += sampling_period_us;
  }

  /*Perform FFT*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

  /*Print out frequency*/
  Serial.println(peak);     //Print out what frequency is the most dominant.

  delay(1000);  //Repeat the process every second OR:
  //while(1);       //Run code once
}
