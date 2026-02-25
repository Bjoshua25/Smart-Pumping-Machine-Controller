#include <SoftwareSerial.h>
#include <avr/wdt.h>

SoftwareSerial lora(10, 11);

// Pins
int lowLevelPin = 2;
int highLevelPin = 3;
int pumpIndicator = 6;
int loraWakePin = 12; // Connected to F8L10A Pin 9 (SLEEP_RQ)

// Heartbeat Variables
unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 900000; // 15 minutes

// Message variables
String message;
String lastMessage = "";

void setup() {
  wdt_disable(); 

  Serial.begin(9600);
  lora.begin(9600);

  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);
  pinMode(pumpIndicator, OUTPUT);
  
  // Initialize LoRa Wake Pin
  pinMode(loraWakePin, OUTPUT);
  digitalWrite(loraWakePin, LOW); // Start in Deep Sleep

  Serial.println("System Started. Heartbeat every 15 mins.");
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();

  // Sense the water level
  bool lowLevelStatus = digitalRead(lowLevelPin);
  bool highLevelStatus = digitalRead(highLevelPin);

  if (lowLevelStatus == LOW) message = "ON_PUMP";
  else if (highLevelStatus == HIGH) message = "OFF_PUMP";
  else message = "Approx. 50%";

  // 2. Decide if we need to transmit (Change in level OR Heartbeat)
  bool isHeartbeat = (millis() - lastHeartbeat > heartbeatInterval);
  
  if (message != lastMessage || isHeartbeat) {
    
    // Reset heartbeat timer if this was a heartbeat event
    if (isHeartbeat) {
      Serial.println(">>> Periodic Heartbeat Triggered <<<");
      lastHeartbeat = millis();
    }

    // STEP A: WAKE LORA
    digitalWrite(loraWakePin, HIGH); 
    delay(100); 

    bool acknowledged = false;
    int retries = 0;

    // STEP B: TRANSMISSION & ACK LOOP
    while (!acknowledged && retries < 3) {
      wdt_reset(); 

      lora.print(message + "\n");
      Serial.print("Sending: "); Serial.print(message);
      Serial.print(" (Attempt "); Serial.print(retries + 1); Serial.println(")");

      unsigned long startWait = millis();
      while (millis() - startWait < 3000) {
        if (lora.available()) {
          String response = lora.readStringUntil('\n');
          response.trim();

          // Standard logic to process ACK and set pump indicator
          if ((message == "ON_PUMP" && response == "ON") || 
              (message == "OFF_PUMP" && response == "OFF") ||
              (message == "ON_PUMP" && response == "Approx_50") ||
              (isHeartbeat && response != "")) { // Heartbeat is happy with any valid response
            
            if (response == "ON") digitalWrite(pumpIndicator, HIGH);
            if (response == "OFF") digitalWrite(pumpIndicator, LOW);
            
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

    if (!acknowledged) Serial.println("Error: No ACK after 3 attempts.");

    // STEP C: RETURN LORA TO SLEEP
    digitalWrite(loraWakePin, LOW); 
    Serial.println("LoRa Sleep Active (Pin 12 LOW)");

    lastMessage = message;
    if (!isHeartbeat) lastHeartbeat = millis(); // Sync heartbeat to last actual transmission
  }

  delay(200);
}