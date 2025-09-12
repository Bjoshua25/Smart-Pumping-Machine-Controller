#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// instantiate LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Arduino Rx=10 Tx=11
SoftwareSerial lora(10, 11);

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
  // Receive Incoming message and print on Serial monitor
  if (lora.available()){
    String receivedMessage = lora.readStringUntil("\n");
    Serial.println("Incoming: " + receivedMessage);


    // Display Pump status on receiving-end lcd
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(receivedMessage);
  }

}