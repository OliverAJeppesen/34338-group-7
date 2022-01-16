#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
//#include <ThingSpeak.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"

const char* ssid = "Olivers Iphone";
const char* pass = "oliver11";
//WiFiClient client;
//unsigned long channelID = 1629314; //your TS channal
//const char * APIKey = "8SH2MRIOCLIKEXVR"; //your TS API
//const char* serverT = "api.thingspeak.com";
#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int accessState = 0;
byte ledPin = D8;

//WiFiServer server(80);
//IPAddress IP(192,168,4,15);
//IPAddress mask = (255, 255, 255, 0);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);   // Initiate a serial communication
  pinMode(ledPin, OUTPUT);
  pinMode(16, OUTPUT);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println();
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Waiting for");
  lcd.setCursor(1, 1);
  lcd.print("key chain...");
  WiFi.persistent( false );
  WiFi.begin(ssid, pass);
  
 Serial.print("connecting");
 while (WiFi.status() != WL_CONNECTED)
 {
 Serial.print(".");
 delay(500);
 }
Serial.println();
 Serial.print("connected: ");
 Serial.println(WiFi.localIP());

 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
if(Firebase.failed())
 {
 Serial.print(Firebase.error());
 }
 else{
 
 Serial.print("Firebase Connected");
 Firebase.setString("Senura/Value","0");
 }

 
 
  

//  WiFi.mode(WIFI_AP);
// WiFi.softAP("Wemos_AP", "Wemos_comm");
// WiFi.softAPConfig(IP, IP, mask);
// server.begin();
// Serial.println();
// Serial.println("accesspoint_bare_01.ino");
// Serial.println("Server started.");
// Serial.print("IP: "); Serial.println(WiFi.softAPIP());
// Serial.print("MAC:"); Serial.println(WiFi.softAPmacAddress());
}

void loop() {
  // put your main code here, to run repeatedly:
  keyChain();
  // comESP();
}

void keyChain() {
  // ThingSpeak.begin(client);
//  client.connect(serverT, 80);
  if ( mfrc522.PICC_IsNewCardPresent() == false) { //If a new PICC placed to RFID reader continue
    return;
  }  if ( mfrc522.PICC_ReadCardSerial() == false) { //Since a PICC placed get Serial and continue
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
    digitalWrite(16, LOW);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Authorized");
    lcd.setCursor(1, 1);
    lcd.print("access");
    accessState = 0;
    if ((Firebase.getInt("ALERT_STATE")==1) || (Firebase.getInt("LISTENING_STATE")==1) ){
    Firebase.setInt("ALERT_STATE",0);
    Firebase.setInt("LISTENING_STATE",0);
    } else {
    if ((Firebase.getInt("LISTENING_STATE")==0) && (Firebase.getInt("ALERT_STATE")==0)){
    Firebase.setInt("LISTENING_STATE",1);
    }
    }
   // ThingSpeak.setField(7, accessState);
   // ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
   // client.stop();
    delay(1000);
    digitalWrite(16, HIGH);
    lcd.setCursor(1, 0);
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  }

  else   {
    Serial.println(" Access denied");
    Serial.println();
    digitalWrite(ledPin, HIGH);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access");
    lcd.setCursor(1, 1);
    lcd.print("denied");
    accessState = 1;
   // ThingSpeak.setField(7, accessState);
    //ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
   // client.stop();
    delay(1000);
    digitalWrite(ledPin, LOW);
    lcd.setCursor(1, 0);
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  }
}

// void comESP() {
// WiFiClient client = server.available();
// if (!client) {return;}
// String request = client.readStringUntil('\r');
// Serial.println("********************************");
// Serial.println("From the station: " + request);
// client.flush();
// Serial.print("Byte sent to the station: ");
// Serial.println(client.println(request + "ca" + "\r"));
//  }
