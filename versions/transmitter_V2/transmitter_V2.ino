#include <SoftwareSerial.h>

// instantiate LoRa for Rx, Tx pins
SoftwareSerial lora(10, 11);

// Define Variables
int lowLevelPin = 2;
int highLevelPin = 3;
int pumpIndicator = 6;

// message variables
String message;
String lastMessage = "";

void setup() {
  // Set up the Serial Monitor
  Serial.begin(9600);
  Serial.println("Pumping Status...");

  // Set up Loral Serial
  lora.begin(9600);

  // Pins Configuration
  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);
  pinMode(pumpIndicator, OUTPUT);
}



void loop() {
  // ======= Sense the water level and send state ======
  // check the water level
  bool lowLevelStatus = digitalRead(lowLevelPin);
  bool highLevelStatus = digitalRead(highLevelPin);

  // Logic pumping Control
  if (lowLevelStatus == LOW){
    message = "ON_PUMP";
  }


  else if (highLevelStatus == HIGH){
    message = "OFF_PUMP";
  }


  else {
    message = "Approx. 50%";
  }

  if (message != lastMessage){
    lora.print(message);
    Serial.println("Message Sent:" + message);
    lastMessage = message;
  }

  delay(200); // debounce



  // ===== Receive Pump state from site ======
  if (lora.available()) {
    String pumpState = lora.readStringUntil("\n");
    pumpState.trim();

    if (pumpState == "ON"){
      digitalWrite(pumpIndicator, HIGH);
      Serial.println("Received ON command");
    }

    else if (pumpState == "OFF") {
      digitalWrite(pumpIndicator, LOW);
      Serial.println("Received OFF command");
    }
  }
}



