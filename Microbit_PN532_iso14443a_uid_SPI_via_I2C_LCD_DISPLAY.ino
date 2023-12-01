/**************************************************************************/
/*!
    @file     iso14443a_uid.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.

    Note that you need the baud rate to be 115200 because we need to print
	out the data and read from the card at the same time!

  This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
  This library works with the Adafruit NFC breakout
  ----> https://www.adafruit.com/products/364

  Check out the links above for our tutorials and wiring diagrams
  These chips use SPI or I2C to communicate.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 line display

#define PN532_SS   (0) // You can choose other pins, pin 0 worked for the Microbit. It also worked with pin 16.
Adafruit_PN532 nfc(PN532_SS);

// Always use hardware SPI when you use a Microbit
// Microbit hardware SPI SCK / SCLK pin: 13
// Microbit hardware SPI MISO pin: 14
// Microbit hardware SPI DATA / MOSI pin: 15
// Also connect the SS pin (see above at #define PN532_SS)
// Do not forget to set the dipswitches on the PN532 board to SPI:
// Set switch #1 to the left
// Set switch #2 to the right

unsigned long serialNo = 0;

void setup(void) {
  lcd.init();                      // initialize the lcd
  lcd.clear();
  lcd.print("NFC kaart lezen...");
  Serial.begin(115200);
  Serial.println("Get iso14443a_uid");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A card");
}

void loop(void) {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    lcd.clear();
    lcd.print("NFC UID Value:");
    lcd.setCursor(0, 1);
    for (uint8_t i = 0; i < uidLength; i++)
    {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
      lcd.print(" "); lcd.print(uid[i], HEX);
    }
    Serial.println("");

    serialNo = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
      serialNo += pow(256, i) * uid[i];
    }
    Serial.print("Serienummer: "); // wordt berekend uit de eerste 4 bytes van het UID (zie boven)
    Serial.println(serialNo); // Unsigned long variables store 32 bits (4 bytes).
    lcd.setCursor(0, 2);
    lcd.print("NFC Serienummer:");
    lcd.setCursor(1, 3);
    lcd.print(serialNo);

    // Wait 1 second before continuing
    delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }
}
