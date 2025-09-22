#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// instantiate LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// instantiate lora, Rx and Tx
SoftwareSerial lora (10, 11);


// Define indicator Variables
int lowLevelIndicator = 2;
int middleLevelIndicator = 3;
int highLevelIndicator = 4;
int pumpStatus = 5;

// Define button pins
int manualButtonOn = 6;
int manualButtonStop = 7;

// Define relay pin
int relayPin = 8;

// Debounce variables
unsigned long lastDebounceTimeOn = 0;
unsigned long lastDebounceTimeOff = 0;
unsigned long debounceDelay = 200; 
bool lastOnState = HIGH;
bool lastOffState = HIGH;


void setup() {
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
  


  // Configure LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Pump Status");
}

void loop() {
  // ========= Get watar level update, and trigger control =======
  if (lora.available()) {
    String receivedMessage = lora.readStringUntil("\n");
    
    if (receivedMessage == "ON_PUMP") {
      digitalWrite(relayPin, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("<50% | PUMP ON");
      digitalWrite(lowLevelIndicator, HIGH);
      digitalWrite(middleLevelIndicator, LOW);
      digitalWrite(highLevelIndicator, LOW);
      digitalWrite(pumpStatus, HIGH);
      lora.print("ON");
      Serial.println("<50% | PUMP ON");
    }

    else if (receivedMessage == "OFF_PUMP") {
      digitalWrite(relayPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("100% | PUMP OFF");
      digitalWrite(lowLevelIndicator, LOW);
      digitalWrite(middleLevelIndicator, LOW);
      digitalWrite(highLevelIndicator, HIGH);
      digitalWrite(pumpStatus, LOW);
      lora.print("OFF");
      Serial.println("100% | PUMP OFF");
    }

    else {
      lcd.setCursor(0, 1);
      lcd.print("Approx. 50%");
      digitalWrite(lowLevelIndicator, LOW);
      digitalWrite(middleLevelIndicator, HIGH);
      digitalWrite(highLevelIndicator, LOW);
      lora.print("Approx_50");
      Serial.println("Approx. 50%");
    }
  }



  // ======== Manual Button Controls =======
  bool onButtonState = digitalRead(manualButtonOn);
  bool offButtonState = digitalRead(manualButtonStop);

  if (onButtonState == LOW && lastOnState == HIGH && (millis()-lastDebounceTimeOn) > debounceDelay){
    lastDebounceTimeOn = millis();
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Manual Pump ON ");
    digitalWrite(pumpStatus, HIGH);
    lora.println("ON");
    Serial.println("Pump Manually ON");
  }
  lastOnState = onButtonState;


  if (offButtonState == LOW && lastOffState == HIGH && (millis() - lastDebounceTimeOff) > debounceDelay) {
    lastDebounceTimeOff = millis();
    digitalWrite(relayPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Manual Pump OFF");
    digitalWrite(pumpStatus, LOW);
    lora.println("OFF");
    Serial.println("Pump Manually OFF");
  }
  lastOffState = offButtonState;
}
