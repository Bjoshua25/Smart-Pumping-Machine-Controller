#include <SoftwareSerial.h>

// Define Tx=11, RX=10 pins
SoftwareSerial lora(10, 11);

void setup() {
  // Baud rate for serial monitor
  Serial.begin(9600);

  // Baud rate for LoRa Serial
  lora.begin(115200);

  // Print a Quick Message
  Serial.println("LoRa F8L10A AT command mode Ready...");
}


void loop() {
  // Computer to Arduino serial monitor then to LoRa Serial
  if (Serial.available()){
    char c = Serial.read();
    lora.write(c);
  }

  // LoRa Serial to Arduino then to Computer
  if (lora.available()){
    char c = lora.read();
    Serial.write(c);
  }
}
