// Set hardwareSerial1 to communicate with LoRa
#define LORA_SERIAL Serial1

void setup() {
  // connect with Serial port for debugging
  Serial.begin(9600);

  while(!Serial); //wait for the debugging serial

  LORA_SERIAL.begin(115200);

  // type your message on serial monitor
  Serial.println("Enter your Command...... ");
  Serial.println("Tip: use AT+VER to check the firmware version: ");
}



void loop() {
  // if LORA is available, read from LoRa and send to Serial Monitor
  if(LORA_SERIAL.available()){
    Serial.write(LORA_SERIAL.read());
  }

  // Read from Serial monitor , send it to LoRa Serial
  if(Serial.available()){
    LORA_SERIAL.write(Serial.read());
  }

}
