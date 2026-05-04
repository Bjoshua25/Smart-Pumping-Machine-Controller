// ==== Receiver: using Arduino Mega MCU

void setup() {
  // Start the main serial port (Serial) for debugging and displaying received data
  Serial.begin(9600);
  while (!Serial);

  // Start the LoRa serial port (Serial1) to communicate with the module
  Serial1.begin(115200);

  Serial.println("Receiver Ready. Waiting for messages...");
}

void loop() {
  // If data is available on the LoRa serial port (Serial1)
  if (Serial1.available()) {
    // Read the data and print it to the main Serial Monitor
    Serial.print("Received: ");
    while (Serial1.available()) {
      Serial.write(Serial1.read());
    }
    Serial.println();
  }
}





