/* This program takes 10 samples from LC + HX711B at
   1-sec interval and then computes the average.
*/

unsigned long x = 0, y=0;
unsigned long dataArray[10];
int j = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(A1, INPUT); //data line  //Yellow cable
  pinMode(A0, OUTPUT);  //SCK line  //Orange cable
}

void loop()
{

  for (int j = 0; j < 10; j++)
  {
    digitalWrite(A0, LOW);//SCK is made LL
    while (digitalRead(A1) != LOW) //wait until Data Line goes LOW
      ;
    {
      for (int i = 0; i < 24; i++)  //read 24-bit data from HX711
      {
        clk();      //generate CLK pulse to get MSB-it at A1-pin
        bitWrite(x, 0, digitalRead(A1));
        x = x << 1;
      }
      clk();  //25th pulse
      Serial.println(x, DEC);
      y = x;
      x = 0;
      delay(1000);
    }
    dataArray[j] = y;
  }

  Serial.println("===averaging process=========");
  unsigned long sum = 0;

  for (j = 0; j < 10; j++)
  {
    sum += dataArray[j];
  }
  Serial.print("Average Count = ");
  sum = sum / 10;
  Serial.println(sum, DEC);
  //float W = (float)0.90*(sum-901002)/946560 + 0.75;//0.005331 * sum - 1146.176;
  //W = (float)W / 1000.00; 
  //Serial.println(W, 2);
}

void clk()
{
  digitalWrite(A0, HIGH);
  digitalWrite(A0, LOW);
}