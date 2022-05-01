#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

// initialize LCD with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Servo toll_gate;
int angle = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //setting up the servo motor
  toll_gate.attach(8);
  toll_gate.write(0);

  //set up rfid
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  //mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
}

void loop() {
  int sensor = analogRead(A0);//get the input from the sensor

  // Print a message to the LCD
  lcd.print("Toll Booth!");
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);

  // Print a message to the LCD.
  lcd.print("Scan card to pay");
  // set the cursor to column 0, line 0
  lcd.setCursor(0, 0);

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    lcd.clear();
    // Print a message to the LCD.
    lcd.print("Card detected!");
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 1);
    // Print a message to the LCD.
    lcd.print("Scanning....");
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 0);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  delay(2000);

  if (content.length() > 0) {
    if ((content == " 7B 7D 1A 1B")||(content == " 1B 27 24 1B")) {
      toll_gate.write(90);
      delay(20);
      
      countdown();

      toll_gate.write(0);
      delay(20);
    }
    
    else {
      Serial.println(content);

      while (!Serial.available()) {
        ;
      }
      String registered = "";
      registered = Serial.readStringUntil('\n');
      int reg = registered.toInt();

      if (registered == "R") {
        Serial.println("bal");

        while (!Serial.available()) {
          ;
        }
        String acc_bal = "";
        acc_bal = Serial.readStringUntil('\n');
        int bal = acc_bal.toInt();
        int toll = 50;

        if (toll <= bal) {
          lcd.clear();
          // Print a message to the LCD.
          lcd.print("Toll charges:");
          // set the cursor to column 0, line 1
          lcd.setCursor(0, 1);
          lcd.print("Ksh. 50");
          lcd.setCursor(0, 0);
          delay(2000);

          // set the cursor to column 0, line 1
          int new_bal = bal - toll;
          Serial.println(new_bal);

          lcd.clear();
          // Print a message to the LCD.
          lcd.print("New balance:");
          // set the cursor to column 0, line 1
          lcd.setCursor(0, 1);
          lcd.print("Ksh. " + String(new_bal));
          lcd.setCursor(0, 0);
          delay(2000);
          lcd.clear();

          angle = 90;

          toll_gate.write(angle);
          delay(20);


          lcd.clear();
          // Print a message to the LCD.
          lcd.print("You're cleared!");
          // set the cursor to column 0, line 1
          lcd.setCursor(0, 1);
          delay(2000);
          lcd.clear();

          countdown();

          angle = 0;
          toll_gate.write(angle);

          delay(20);
        }

        else {
          lcd.clear();
          lcd.print("Insufficient!!!!");
          // set the cursor to column 0, line 1
          lcd.setCursor(0, 1);
          lcd.print("Top up account");
          lcd.setCursor(0, 0);
          delay(2000);
          lcd.clear();
        }
      }
      else {
        lcd.clear();
        // Print a message to the LCD.
        lcd.print("Unregistered card!!!");
        // set the cursor to column 0, line 1
        lcd.setCursor(0, 1);
        lcd.print("Call care agent");
        lcd.setCursor(0, 0);
        delay(2000);
        lcd.clear();
      }
    }
  }
}

void countdown() {
  int countdown = 20;
  while (countdown >= 0) {
    // Print a message to the LCD.
    lcd.print("Closing in 20s");
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 1);
    // Print a message to the LCD.
    lcd.print(countdown);
    // set the cursor to column 0, line 1
    lcd.setCursor(0, 0);

    countdown -= 1;

    delay(1000);
    lcd.clear();
  }
}
