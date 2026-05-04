#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include "LowPower.h"


SoftwareSerial lora(10, 11);


// Pins
int lowLevelPin = 2;
int highLevelPin = 3;
int pumpIndicator = 6;
int loraWakePin = 12;  // connected to F8L10A pin 9 (SLEEP_RQ)


// Define Heartbeat Variables
int sleepCycles = 0;
const int heartbeatThreshold = 110;  // 113 cycles * 8 seconds = 15 mins


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

}




void loop() {
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
  bool isHeartbeat = (sleepCycles >= heartbeatThreshold);


  if (message != lastMessage || isHeartbeat) {

    
    // Wake LoRa Radio
    digitalWrite(loraWakePin, HIGH);
    delay(100); // Give LoRa module time to stabilize

    if (isHeartbeat) {
      Serial.println("Heartbeat Triggered!");
    }



    // Transmission and Ack Loop
    bool acknowledged = false;
    int retries = 0;

    while (!acknowledged && retries < 3) {
      wdt_enable(WDTO_8S); // Enable WDT safety during transmission
      wdt_reset();

      lora.print(message + "\n");
      Serial.print("Sending: "); Serial.println(message);


    // Wait 3 seconds for acknowledgement
      unsigned long startWait = millis();
      while (millis() - startWait < 3000) {
        if (lora.available()) {
          String response = lora.readStringUntil('\n');
          response.trim();

          // If the controller responds, sync the LED indicator
          if (response == "ON") {
            digitalWrite(pumpIndicator, HIGH);
            acknowledged = true;
          } else if (response == "OFF") {
            digitalWrite(pumpIndicator, LOW);
            acknowledged = true;
          } else if (response == "Approx_50") {
            acknowledged = true;
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


    // Put LoRa back to sleep
    digitalWrite(loraWakePin, LOW);
    Serial.println("LoRa Sleeping. MCU Sleeping...");
    Serial.flush(); // Ensure serial prints before sleep

    // Reset tracking variables
    lastMessage = message;
    sleepCycles = 0; // Reset counter after any transmission
    wdt_disable(); // Disable WDT to allow library to manage it
  }




  // Put ATmega328P to Deep Sleep for 8 seconds... Current consumption drops to ~20uA here.
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  // Once it wakes up (after 8s), increment the counter and loop again
  sleepCycles++;


}
