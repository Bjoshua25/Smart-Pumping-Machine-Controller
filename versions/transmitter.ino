// include libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// LoRa on pins Rx=10, Tx=11
SoftwareSerial lora(10, 11);

// Instantiate The Lcd library
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the pin variables
const int lowLevelPin = 2;
const int highLevelPin = 3;
const int relayPin     = 7;

// Track last message to avoid redundant updates
String lastMessage = "";

// Set manualOverride State initially to false
bool manualOverride = false;

// define the message variable
String message;

void setup() {
  // Begin Serial monitor for debugging
  Serial.begin(9600);
  lora.begin(9600);
  Serial.println("Pump Control Initialized");
  Serial.println("-------------------------");

  // Configure Pin Modes
  // pins in INPUT_PULLUP are High by default
  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);

  // Initial Control state of the pumping machine
  digitalWrite(relayPin, LOW);

  // Configure the LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pump Status");
}




void loop() {
  // Is there manual override command from the Receiver?
  if (lora.available()){
    String command = lora.readStringUntil("\n");
    command.trim();

    // Analyze the received command
    if (command == "ON"){
      manualOverride = true;
      digitalWrite(relayPin, HIGH);
      lora.print("Pump ON: Manual");
      Serial.println("Manually Pumping is ON...");
    }
    else if (command == "OFF") {
      manualOverride = true;
      digitalWrite(relayPin, LOW);
      lora.print("Pump OFF: Manual");
      Serial.println("Pumping is Manually OFF");
    }
  }


  // Reset Back to Automatic Algorithm (After The System operated under Manual command)
  if (manualOverride) {
    // check the level of the water
    bool highLevelStatus = digitalRead(highLevelPin);

    // At highest level, turn off the pump and reset back to Automatic mode
    if (highLevelStatus == LOW){
      manualOverride = false; // return to Auto mode                     
      digitalWrite(relayPin, LOW);
      message = "100% | PUMP OFF    ";
      Serial.println("Reset To Auto...");
      lora.print("AUTO:100% | Pump OFF ");
    }
  }


  // Follow the Automatic system Algorithm (If Pump is not previously in Manual command)
  if (!manualOverride) {
    // Reading Probe status of the Cable Sensors or Probes
    bool lowLevelStatus  = digitalRead(lowLevelPin);
    bool highLevelStatus = digitalRead(highLevelPin);


    // ======= Control logic for the Pumping Machine ========
    // Turn ON when lowLevelStatus is HIGH; that is, when water is below this cable sensor
    if (lowLevelStatus == HIGH) {
      digitalWrite(relayPin, HIGH);
      message = "<50% | PUMP ON   ";
    }
    // Turn OFF when highLevelSensor is LOW; that is, when water has touched the top-most cable sensor
    else if (highLevelStatus == LOW) {
      digitalWrite(relayPin, LOW);
      message = "100% | PUMP OFF    ";
    }
    else {
      message = "Apprx. 50%       ";
    }
  }


  

  // Only update if the message changed
  if (message != lastMessage) {
    Serial.println(message);

    lcd.setCursor(0, 1);
    lcd.print("                "); // clear line
    lcd.setCursor(0, 1);
    lcd.print(message);

    lora.println(message);

    lastMessage = message;
  }

  delay(200); // debounce
}
