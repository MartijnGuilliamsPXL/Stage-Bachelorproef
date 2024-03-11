#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define LOADCELL1_DOUT_PIN  4
#define LOADCELL1_SCK_PIN  3

#define LOADCELL2_DOUT_PIN  6
#define LOADCELL2_SCK_PIN  5

#define LOADCELL3_DOUT_PIN  8
#define LOADCELL3_SCK_PIN  7

#define LOADCELL4_DOUT_PIN  10
#define LOADCELL4_SCK_PIN  9

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

float calibration_factor = -94850; //airpods(42g) -115050 //motor(195g) -78600 //lood(2883.3g)

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale3.begin(LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
  scale4.begin(LOADCELL4_DOUT_PIN, LOADCELL4_SCK_PIN);

  scale1.set_scale();
  scale1.tare();	//Reset the scale to 0
  scale2.set_scale();
  scale2.tare();	//Reset the scale to 0
  scale3.set_scale();
  scale3.tare();	//Reset the scale to 0
  scale4.set_scale();
  scale4.tare();	//Reset the scale to 0

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  long zero_factor3 = scale1.read_average(); //Get a baseline reading
  long zero_factor4 = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor1);
}

void loop() {

  scale1.set_scale(calibration_factor); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor); //Adjust to this calibration factor
  scale3.set_scale(calibration_factor); //Adjust to this calibration factor
  scale4.set_scale(calibration_factor); //Adjust to this calibration factor

  float weight_kg1 = scale1.get_units() * 0.453592; // Convert lbs to Kg
  float weight_kg2 = scale2.get_units() * 0.453592; // Convert lbs to Kg
  float weight_kg3 = scale3.get_units() * 0.453592; // Convert lbs to Kg
  float weight_kg4 = scale4.get_units() * 0.453592; // Convert lbs to Kg

  float weight_g1 = weight_kg1 * 1000; // Convert Kg to g
  float weight_g2 = weight_kg2 * 1000; // Convert Kg to g
  float weight_g3 = weight_kg3 * 1000; // Convert Kg to g
  float weight_g4 = weight_kg4 * 1000; // Convert Kg to g

  // Thrust
  float Thrust_N = (weight_g1 + weight_g2) * 9.81;

  // Torque
  float force1_newtons = weight_kg3 * (9.81);
  float force2_newtons = weight_kg4 * (9.81);
  float avgForce_newtons = (force1_newtons + force2_newtons) / 2;
  float torque_Nm = avgForce_newtons * 0.03365;




  //Serial.print("Scale1Thrust:");
  //Serial.print(weight_g1, 1);
  //Serial.print(",");
  //Serial.print("Scale2Thrust:");
  //Serial.print(weight_g2, 1);

  /*
  Serial.print("Thrust:");
  Serial.print((weight_g1 + weight_g2), 1);
  Serial.print(",");
  */

  Serial.print("Thrust N:");
  Serial.print(Thrust_N, 2);
  Serial.print(",");

  /*
  Serial.print("Scale3Torque:");
  Serial.print(weight_g3, 1);
  Serial.println(",");
  Serial.print("Scale4Torque:");
  Serial.print(weight_g4, 1);
  */

  Serial.print("Torque Nm:");
  Serial.print(torque_Nm,6);

  //Serial.print(" calibration_factor: ");
  //Serial.print(calibration_factor);
  //moment lenth 0.03365m => 33.65mm
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


