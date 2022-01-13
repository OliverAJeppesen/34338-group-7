#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "Nikklas iphone";
const char* pass = "niklas01";
WiFiClient client;
unsigned long channelID = 1629314; //your TS channal
const char * APIKey = "8SH2MRIOCLIKEXVR"; //your TS API
const char* server = "api.thingspeak.com";
#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int accessState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println();
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Waiting for");
  lcd.setCursor(1, 1);
  lcd.print("key chain...");
  WiFi.begin(ssid, pass);
}

void loop() {
  // put your main code here, to run repeatedly:
 keyChain();
}

void keyChain() {
  ThingSpeak.begin(client);
  client.connect(server, 80);
  if ( mfrc522.PICC_IsNewCardPresent() == false) { //If a new PICC placed to RFID reader continue
    return;
  }
  if ( mfrc522.PICC_ReadCardSerial() == false) { //Since a PICC placed get Serial and continue
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "7A C4 97 80") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Authorized");
    lcd.setCursor(1, 1);
    lcd.print("access");
    accessState = 0;
    ThingSpeak.setField(7, accessState);
    ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
    client.stop();
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  }

  else   {
    Serial.println(" Access denied");
    Serial.println();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access");
    lcd.setCursor(1, 1);
    lcd.print("denied");
    accessState = 1;
    ThingSpeak.setField(7, accessState);
    ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
    client.stop();
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  }
}
