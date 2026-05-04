#include <SoftwareSerial.h>
#include <avr/wdt.h>

// instantiate LoRa for Rx, Tx pins
SoftwareSerial lora(10, 11);

// Define Variables
int lowLevelPin = 2;
int highLevelPin = 3;
int pumpIndicator = 6;

// Heartbeat Interval Variables
unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 900000;     // Equivalent to 15 minutes

// message variables
String message;
String lastMessage = "";

void setup() {
  // Always Disable WDT at first to prevent Boot Loops
  wdt_disable(); 


  // Set up the Serial Monitor
  Serial.begin(9600);
  Serial.println("Pumping Status...");

  // Set up Loral Serial
  lora.begin(9600);

  // Pins Configuration
  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);
  pinMode(pumpIndicator, OUTPUT);


  // Enable watch dog timer for 8 seconds
  wdt_enable(WDTO_8S);
}



void loop() {
  // Reset watch dog timer
  wdt_reset();


  // ================= Sense the water level and send state =======================
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






  // ====================== Transmission and Acknowledgement Logic ==================
  if (message != lastMessage){
    bool acknowledged = false;
    int retries = 0;

    // Transmit to Controller if retry less than 3
    while (!acknowledged && retries < 3){
      wdt_reset(); 

      lora.print(message);
      Serial.print("Sending: "); Serial.println(message);
      Serial.print(" (Attempt "); Serial.print(retries + 1); Serial.println(")");

      // Receive acknowledgement from controller
      unsigned long startWait = millis();
      while (millis() - startWait < 2000){
        if (lora.available()){
          String response = lora.readStringUntil("\n");
          response.trim();


          if (message == "ON_PUMP" && response == "ON"){
            digitalWrite(pumpIndicator, HIGH);
            acknowledged = true;
            break;
          }

          else if (message == "OFF_PUMP" && response == "OFF"){
            digitalWrite(pumpIndicator, LOW);
            acknowledged = true;
            break;
          }

          else if (message == "ON_PUMP" && response == "Approx_50"){
            acknowledged = true;
            break;
          }
        }
      }


      // if There is no Acknowldgement; Update No of Retries
      if (!acknowledged){
        retries++;
        Serial.println("No ACK, retrying...");
        delay(100);
      }
    }

    if (!acknowledged) Serial.println(("Error: No ACK after 3 attempts."));
    lastMessage = message;
  }







  // ====================== Receive Pump state from site ======================
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

  delay(200);


  // ============ Periodic Heart Beat Logic =================================
  if (millis() - lastHeartbeat > heartbeatInterval) {
    lastMessage = "";
    lastHeartbeat = millis();
  }
}



