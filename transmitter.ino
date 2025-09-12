// include libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// LoRa on pins 10,11
SoftwareSerial lora(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the pin variables
const int lowLevelPin = 2;
const int highLevelPin = 3;
const int relayPin     = 7;

// Track last message to avoid redundant updates
String lastMessage = "";

void setup() {
  // Begin Serial monitor for debugging
  Serial.begin(9600);
  lora.begin(9600);
  Serial.println("Pump Control Initialized");
  Serial.println("-------------------------");

  // Configure Pin Modes
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
  // Reading Probe status of the Cable Sensors
  bool lowLevelStatus  = digitalRead(lowLevelPin);
  bool highLevelStatus = digitalRead(highLevelPin);

  String message;

  // ==== Control logic for the Pumping Machine ====
  // Turn ON when lowLevelStatus is HIGH
  if (lowLevelStatus == HIGH) {
    digitalWrite(relayPin, HIGH);
    message = "<50% | PUMP ON";
  }
  else if (highLevelStatus == LOW) {
    digitalWrite(relayPin, LOW);
    message = "100% | PUMP OFF";
  }
  else {
    message = "Apprx. 50%";
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
