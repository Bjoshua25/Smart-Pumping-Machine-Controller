// include Libraries
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// instantiate LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Arduino Rx=10 Tx=11
SoftwareSerial lora(10, 11);

// Button Pin
int buttonPin = 6;

// Button state
bool manualOverride = false;

// ======= Sanitize Received Msg =========
String sanitize (String rawMsg) {
  String cleanMsg = "";

  // iterate over individual characters
  for (unsigned int i = 0; i < rawMsg.length(); i++){
    char c = rawMsg[i];
    if (c >= 32 && c <= 126){
      cleanMsg += c;
    }
  }
  return cleanMsg;
}


// ====== Padding the lcd message ========
String padTo16 (String msg) {
  while (msg.length() < 16) {
    msg += " "; // add spaces until it is 16
  }
  return msg;
}


void setup() {
  // Serial UART
  Serial.begin(9600);
  // LoRa Serial 
  lora.begin(9600); 

  Serial.println("Receiver Ready..");

  // Set the Button Pin Mode... By default the btton reads HIGH
  pinMode(buttonPin, INPUT_PULLUP);

  // configure and set up Lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pump Status");
}


void loop() {
  // refreshes the first row of the Lcd
  lcd.setCursor(0, 0);
  lcd.print("Pump Status    ");


  // ====== Send the button state to the transmitter, for Manual override ========
  if (digitalRead(buttonPin) == LOW){
    // Debounce, ensure to wait for 50 ms to be sure of the button state
    delay(50);

    // Recheck again...
    if (digitalRead(buttonPin) == LOW){
      manualOverride = !manualOverride;
      if (manualOverride){
        lora.print("ON");
        Serial.println("Sent: ON");
        lcd.setCursor(0, 1);
        lcd.print("Manual Pump ON ");
      } 
      else {
        lora.print("OFF");
        Serial.println("Sent: OFF");
        lcd.setCursor(0, 1);
        lcd.print("Manual Pump OFF");
      }
    }
    
    // wait until the push button is released
    while(digitalRead(buttonPin) == LOW);
  }


  // ===== Receive Incoming message and clean it using the sanitize func=====
  if (lora.available()){
    String receivedMessage = lora.readStringUntil("\n");
    receivedMessage.trim();
    receivedMessage = sanitize(receivedMessage);

    // Only print message when length is more than 0
    if (receivedMessage.length() > 0) {

      // Checking the Auto Message Alone:
      if (receivedMessage.startsWith("AUTO:")) {
        // Strip "Auto:" from the Msg, leaving "100% | Pump OFF"
        String autoMsg = receivedMessage.substring(5);
        autoMsg = padTo16(autoMsg);
        Serial.println("Incoming: " + autoMsg);
        lcd.setCursor(0, 1);
        lcd.print(autoMsg);
      }

      else { 
        if (receivedMessage.length() < 16) {
          receivedMessage = padTo16(receivedMessage);
        } 

        receivedMessage = padTo16(receivedMessage);
        Serial.println("Incoming: " + receivedMessage);
        lcd.setCursor(0, 1);
        lcd.print(receivedMessage);
      }
      
    } 
  }
}