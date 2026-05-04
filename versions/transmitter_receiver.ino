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

  Serial.println("Transmitter Ready to send...");

  // configure and set up Lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pump Status");
}

void loop() {
  // Transmit message from Serial to LoRa Node 2
  if (Serial.available()){
    String message = Serial.readStringUntil("\n");
    lora.println(message);
    Serial.println("Successfully Sent: " + message);
  }

  // Receive Incoming message and print on Serial monitor
  if (lora.available()){
    String receivedMessage = lora.readStringUntil("\n");
    Serial.println("Incoming: " + receivedMessage);
  }
}





// ===============================================================
// Trying with Arduino Mega

// void setup() {
//   // Serial UART
//   Serial.begin(9600);
//   // Serial1 baud rate
//   Serial1.begin(115200);

//   Serial.println("Transmitter Ready to Send...");
// }

// void loop() {
//   String message = "Hello From the Transmitter";

//   // Send message to LoRa
//   Serial1.println(message);
//   // confirm sent message on serial monitor
//   Serial.println("Successfully Sent: " + message);

//   // Resend Message every 3 seconds
//   delay(3000);

// }
