#include <SoftwareSerial.h>
SoftwareSerial lora(10, 11); // RX=10, TX=11 (change if you used other pins)

void setup() {
  Serial.begin(9600);     // USB to PC
  lora.begin(9600);       // LoRa default — change if you know the module baud

}

void loop() {
  while (Serial.available()) {         // data from PC -> LoRa
    lora.write(Serial.read());
  }
  while (lora.available()) {           // data from LoRa -> PC
    Serial.write(lora.read());
  }
}
