// include Libraries
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// instantiate LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Arduino Rx=10 Tx=11
SoftwareSerial lora(10, 11);



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

  // Receive Incoming message and clean it using the sanitize func
  if (lora.available()){
    String receivedMessage = lora.readStringUntil("\n");
    receivedMessage.trim();
    receivedMessage = sanitize(receivedMessage);

    // Only print message when length is more than 0
    if (receivedMessage.length() > 0 && receivedMessage.length() < 16){
      Serial.println("Incoming: " + receivedMessage);

      // Pad the message to 16 characters
      receivedMessage = padTo16(receivedMessage);

      // Display Pump status on receiving-end lcd
      lcd.setCursor(0, 1);
      lcd.print(receivedMessage);
    } 
  }
}