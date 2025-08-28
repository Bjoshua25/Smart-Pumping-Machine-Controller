// include libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pin variables
const int lowLevelPin = 2;
const int highLevelPin = 3;
const int relayPin = 7;

// Instantiate the LCD module
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Begin Serial monitor for LoRa
  Serial.begin(9600);
  Serial.println("Pump Control Initialized");
  Serial.println("-------------------------");

  // Configure Pin Modes
  // Digital pin with Input_PullUp are HIGH by default
  pinMode(lowLevelPin, INPUT_PULLUP);
  pinMode(highLevelPin, INPUT_PULLUP);

  // Configuring output Pin
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
  bool lowLevelStatus = digitalRead(lowLevelPin);
  bool highLevelStatus = digitalRead(highLevelPin);

  //==== Control logic for the Pumping Machine ====
  // Turn ON when lowlevelStatus is HIGH
  if(lowLevelStatus == HIGH){
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("<50% | PUMP ON   ");
    Serial.println("<50% | PUMP ON");
  }
  else if(highLevelStatus == LOW){
    digitalWrite(relayPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("100% | PUMP OFF    ");
    Serial.println("100% | PUMP OFF");
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("Apprx. 50%");
    Serial.println("Apprx. 50%");
  }

  // Delay for a little time before reading all over again
  delay(500);
}