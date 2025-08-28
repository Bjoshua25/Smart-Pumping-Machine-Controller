// ====== Using arduino Uno as the Transmitter =====

#include <SoftwareSerial.h> // because you are using UNO

SoftwareSerial loraSerial(10, 11);

void setup() {
  // start the main serial port
  Serial.begin(9600);

  while(!Serial);

  loraSerial.begin(115200);

  Serial.println("Transmitter Ready; Sending data every 5 seconds");

}

void loop() {
  // check if the LoRa module is ready to read
  if(loraSerial.availableForWrite()){

    loraSerial.println("Hello from Arduino Uno!");
    loraSerial.println("Message Sent.");

  }
  delay(500);
}
