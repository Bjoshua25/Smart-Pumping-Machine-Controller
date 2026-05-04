#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <EEPROM.h>


// instantiate lora, Rx and Tx
SoftwareSerial lora (10, 11);


// Define indicator Variables
const byte lowLevelIndicator = 2;
const byte middleLevelIndicator = 3;
const byte highLevelIndicator = 4;
const byte pumpStatus = 5;

// Define button pins
const byte manualButtonOn = 6;
const byte manualButtonStop = 7;

// Define relay pin
const byte relayPin = 8;

// EEPROM Address to store pump state
const int pumpStateAddress = 0;

// Debounce variables
unsigned long lastDebounceTimeOn = 0;
unsigned long lastDebounceTimeOff = 0;
unsigned long debounceDelay = 200; 
bool lastOnState = HIGH;
bool lastOffState = HIGH;


void setup() {
  // Disable watch dog timer at first to prevent boot loops
  wdt_disable();


  // Set up Serial monitor
  Serial.begin(9600);
  Serial.println("Pump Status");

  // Set up lora Serial
  lora.begin(9600);

  // Configure Pin Modes
  pinMode(relayPin, OUTPUT);
  pinMode(manualButtonOn, INPUT_PULLUP);
  pinMode(manualButtonStop, INPUT_PULLUP);

  // Configure indicators
  pinMode(lowLevelIndicator, OUTPUT);
  pinMode(middleLevelIndicator, OUTPUT);
  pinMode(highLevelIndicator, OUTPUT);
  pinMode(pumpStatus, OUTPUT);
  


  // =========== EEPROM System Restore =============
  // read the last saved state from Address 0
  byte lastState = EEPROM.read(pumpStateAddress);

  // use the custom EEPROM function
  setPumpState(lastState);
  

  // Enable Watch Dog timer after 8 seconds of inactiveness
  wdt_enable(WDTO_8S);
}



void loop() {
  // Reset the watch dog timer
  wdt_reset();


  // ========= Get watar level update, and trigger control =======
  if (lora.available()) {
    String receivedMessage = lora.readStringUntil("\n");
    receivedMessage.trim();
    
    if (receivedMessage == "ON_PUMP") {
      setPumpState(HIGH);
      digitalWrite(lowLevelIndicator, HIGH);
      digitalWrite(middleLevelIndicator, LOW);
      digitalWrite(highLevelIndicator, LOW);
      lora.println("ON");
      Serial.println("<50% | PUMP ON");
    }

    else if (receivedMessage == "OFF_PUMP") {
      setPumpState(LOW);
      digitalWrite(lowLevelIndicator, LOW);
      digitalWrite(middleLevelIndicator, LOW);
      digitalWrite(highLevelIndicator, HIGH);
      lora.println("OFF");
      Serial.println("100% | PUMP OFF");
    }

    else {
      digitalWrite(lowLevelIndicator, LOW);
      digitalWrite(middleLevelIndicator, HIGH);
      digitalWrite(highLevelIndicator, LOW);
      lora.println("Approx_50");
      Serial.println("Approx. 50%");
    }
  }



  // ======== Manual Button Controls =======
  bool onButtonState = digitalRead(manualButtonOn);
  bool offButtonState = digitalRead(manualButtonStop);

  if (onButtonState == LOW && lastOnState == HIGH && (millis()-lastDebounceTimeOn) > debounceDelay){
    lastDebounceTimeOn = millis();
    setPumpState(HIGH);
    lora.println("ON");
    Serial.println("Pump Manually ON");
  }
  lastOnState = onButtonState;


  if (offButtonState == LOW && lastOffState == HIGH && (millis() - lastDebounceTimeOff) > debounceDelay) {
    lastDebounceTimeOff = millis();
    setPumpState(LOW);
    lora.println("OFF");
    Serial.println("Pump Manually OFF");
  }
  lastOffState = offButtonState;
}




// ========== Custom Function to Handle Relay Switching and EEPROM updating ====================
void setPumpState(bool state){
  digitalWrite(relayPin, state);
  digitalWrite(pumpStatus, state);

  // Update EEPROM when the value is different from what exists in the memory
  EEPROM.update(pumpStateAddress, state);
}