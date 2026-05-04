#include <SoftwareSerial.h>
#include <avr/wdt.h>

SoftwareSerial lora(10, 11);

// Pins
int lowLevelPin = 2;
int highLevelPin = 3;
int pumpIndicator = 6;
int loraWakePin = 12;  // connected to F8L10A pin 9 (SLEEP_RQ)


// Define Heartbeat Variables
unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 900000;  // 15 minutes


// Define Message Variables
String message;
String lastMessage= "";


void setup() {
  // Temporary stop the watch dog timer to allow for bootloading...
  wdt_disable();

  // Set baudrate for the UART comm devices
  Serial.begin(9600);
  lora.begin(9600);


  // define PinModes 
  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);
  pinMode(pumpIndicator, OUTPUT);
  pinMode(loraWakePin, OUTPUT);


  // At first, keep LoRa in its Deep Sleep Mode
  digitalWrite(loraWakePin, LOW);

  Serial.println("System Started... ");

  // start up the watch dog timer and set countdown to 8_sec
  wdt_enable(WDTO_8S);

}





void loop() {
  // reset the 8-sec countdown to avoid automatic restart
  wdt_reset();

  
  // read water level in the reservoir
  bool lowLevelStatus = digitalRead(lowLevelPin);
  bool highLevelStatus = digitalRead(highLevelPin);




  // ---------------------- WATER LEVEL CONTROL LOGIC --------------------------
  if (lowLevelStatus == LOW) {
    message = "ON_PUMP";
  }
  
  else if (highLevelStatus == HIGH){
    message = "OFF_PUMP";
  }

  else {
    message = "Approx. 50%";
  }




  // ------------ TRANSMIT: BASED ON "LEVEL-CHANGE" OR "HEARTBEAT" -----------------
  bool isHeartbeat = (millis() - lastHeartbeat > heartbeatInterval);


  if (message != lastMessage || isHeartbeat){
    

    // Reset heartbeat timer if this a heartbeat event
    if (isHeartbeat) {
      Serial.println("Periodic heartbeat Triggered!");
      lastHeartbeat = millis();
    }


    // step_1: Wake LoRa
    digitalWrite(loraWakePin, HIGH);
    delay(100);


    // step_2: Transmission and Ack Loop
    bool acknowledged = false;
    int retries = 0;

    while (!acknowledged && retries < 3) {
      wdt_reset();

      lora.print(message + "\n");
      Serial.println("Sending: "); Serial.print(message);
      Serial.println("( Attempt "); Serial.print(retries + 1); Serial.println(" )");
    

      // step_3: wait for 3 seconds for an acknowlegement message
      unsigned long startWait = millis();
      while (millis() - startWait < 3000) {
        if (lora.available()) {
          String response = lora.readStringUntil('\n');
          response.trim();

          // logic for acknowlegemnet message and Pump Indicator
          if (
            (message == "ON_PUMP" && response == "ON") ||
            (message == "OFF_PUMP" && response == "OFF") ||
            (message == "ON_PUMP" && response == "Approx_50") ||
            (isHeartbeat && response != ""))
            {
              if (response == "ON") {
                digitalWrite(pumpIndicator, HIGH);
              }

              if (response == "OFF") {
                digitalWrite(pumpIndicator, LOW);
              }

              acknowledged = true;
              break;
            }
          }
        } 

        if (!acknowledged) {
          retries++;
          Serial.println("No ACK, retrying...");
          delay(200);
        }
    }

    if (!acknowledged) {
      Serial.println("Error: No ACK after 3 attempts.");
    }

    // step 4: Return LoRa to Sleep
    digitalWrite(loraWakePin, LOW);
    Serial.println("LoRa Sleep Active (Pin 12 LOW)");

    lastMessage = message;
    lastHeartbeat = millis();
  }

  delay(200);

}