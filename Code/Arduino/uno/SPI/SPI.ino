#include <SPI.h>

volatile bool received;
volatile byte data;

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(MISO, OUTPUT); // Set MISO as OUTPUT (Slave Out)
  
  // Enable SPI as slave
  SPCR |= _BV(SPE);
  
  // Turn on SPI interrupt
  SPCR |= _BV(SPIE);
  
  received = false;
  data = 0;
}

// SPI interrupt routine
ISR (SPI_STC_vect) {
  data = SPDR; // Read byte from SPI Data Register
  received = true;
}

void loop() {
  if (received) {
    Serial.print("Received: ");
    Serial.println(data, DEC); // Print received data as decimal
    received = false;
  }
}
