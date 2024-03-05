#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

#define LOADCELL2_DOUT_PIN  6
#define LOADCELL2_SCK_PIN  5

HX711 scale;
HX711 scale2;

float calibration_factor = -94850; //airpods(42g) -115050 //motor(195g) -78600 //lood(2883.3g)

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale.set_scale();
  scale.tare();	//Reset the scale to 0
  scale2.set_scale();
  scale2.tare();	//Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading:");
  float weight_kg = scale.get_units() * 0.453592; // Convert lbs to Kg
  float weight_g = weight_kg * 1000; // Convert Kg to g
  Serial.print(weight_g, 1); // Display weight in grams with one decimal place
  //Serial.print(" g");

  //Serial.print(",");
  Serial.println();

   Serial.print("Reading2:");
  float weight_kg2 = scale2.get_units() * 0.453592; // Convert lbs to Kg
  float weight_g2 = weight_kg2 * 1000; // Convert Kg to g
  Serial.print(weight_g2, 1); // Display weight in grams with one decimal place
  //Serial.print(" g");

  //Serial.print(" calibration_factor: ");
  //Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 100;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 100;
  }
}
