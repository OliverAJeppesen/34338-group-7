/*
  keyChain method

  Turns on or off the security system if the right key chain is scanned by the RFID scanner. In addition it writes access information on a
  LCD screen and transmits access information to a realtime firebase database for the other parts of the program to use.

  Main responsible for this code:
  Oliver Aagaard Jeppsen, s204180

  last modified 19 January 2022
  by Oliver Aagaard Jeppesen, s204180

*/

#include <SPI.h> // Libraries needed for the RFID
#include <MFRC522.h>

#include <ESP8266WiFi.h> // Libraries needed for the wifi connection and the connection to firebase.
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"

#include <NTPClient.h> // Libraries needed for to retrieve the exact time from a NTP server.
#include <WiFiUdp.h>

#include <LiquidCrystal_I2C.h> // Library needed for theLCD screen.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize LCD screen to a two row display.

const char* ssid = "Hulubulu"; // SSID and password for wifi connection.
const char* pass = "Thorupstrand17";

#define SS_PIN D4 // Initialize MFRC522 instance with given pins .
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);

byte ledPin = D8; // Initialize led pin.

const long utcOffsetInSeconds = 3600; // Define NTP Client to get time and the day of the week. Receiving from the webpage "pool.ntp.org".
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup() { // put your setup code here, to run once:
  Serial.begin(115200);   // Initiate a serial communication

  pinMode(ledPin, OUTPUT); // Define led output.
  pinMode(16, OUTPUT);

  SPI.begin(); // Initiate MFRC522 and the SPI bus.
  mfrc522.PCD_Init();

  lcd.init(); // Initiate the LCD screen.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Waiting for");
  lcd.setCursor(1, 1);
  lcd.print("key chain...");

  WiFi.persistent( false ); // Start the wifi connection.
  WiFi.begin(ssid, pass);

  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) // Waiting for wifi connection.
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(); // Message that wifi is connected.
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  timeClient.begin(); // Start the time client for receiving time stamps.

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Connect to the right database.
  if (Firebase.failed())
  {
    Serial.print(Firebase.error());
  }
  else {
    Serial.println("Firebase Connected");
  }

  Serial.println(); // Tells that RFID is ready to read.
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop() { // put your main code here, to run repeatedly:
  keyChain(); // Calling the keyChain method.
}

void keyChain() {
  if ( mfrc522.PICC_IsNewCardPresent() == false) { //If a key chain or whitecard is present to the RFID reader then continue.
    return;
  }  if ( mfrc522.PICC_ReadCardSerial() == false) {  //If a key chain or whitecard serial is read then continue.
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)  //Print the UID in HEX on serial monitor and have the UID ready for access check.
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "7A C4 97 80") //Check if the used UID of the key chain or whitecard is authorized to access the system.
  {
    Serial.println("Authorized access"); // Print that access is granted on both serial monitor and LCD screen.
    Serial.println();
    digitalWrite(16, LOW);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Authorized");
    lcd.setCursor(1, 1);
    lcd.print("access");

    timeClient.update(); // Update the time stamp from the NTP server.

    Firebase.setString("lastAccessID", content.substring(1) + ", " + name(content.substring(1))); // set access ID, Status and Time on the firebase database.
    Firebase.setString("accessStatus", "Authorized access");
    Firebase.setString("accessTime", String(daysOfTheWeek[timeClient.getDay()]) + ", " + String(timeClient.getHours())
    + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
    if ((Firebase.getInt("ALERT_STATE") == 1) || (Firebase.getInt("LISTENING_STATE") == 1) ) { // If the system is off then turn it on and if it is on then turn it off.
      Firebase.setInt("ALERT_STATE", 0);
      Firebase.setInt("LISTENING_STATE", 0);
    } else {
      if ((Firebase.getInt("LISTENING_STATE") == 0) && (Firebase.getInt("ALERT_STATE") == 0)) {
        Firebase.setInt("LISTENING_STATE", 1);
      }
    }
    delay(1000);

    digitalWrite(16, HIGH);
    lcd.setCursor(1, 0); // Show on the LCD screen that the scanner is ready again.
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  } else   {
    Serial.println(" Access denied"); // This case happens when the UID of the key chain or the whitecard is NOT authorized to access the system.
    Serial.println();
    digitalWrite(ledPin, HIGH); // Print that access is denied on both serial monitor, LCD screen and indicate it with a red led.
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access");
    lcd.setCursor(1, 1);
    lcd.print("denied");

    timeClient.update();  // Update the time stamp from the NTP server.

    Firebase.setString("lastAccessID", content.substring(1)); // set access ID, Status and Time on the firebase database.
    Firebase.setString("accessStatus", "Acces denied");
    Firebase.setString("accessTime", String(daysOfTheWeek[timeClient.getDay()]) + ", " + String(timeClient.getHours())
    + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
    delay(1000);

    digitalWrite(ledPin, LOW);
    lcd.setCursor(1, 0); // Show on the LCD screen that the scanner is ready again.
    lcd.print("Waiting for");
    lcd.setCursor(1, 1);
    lcd.print("key chain...");
  }
}

String name (String id) { // Method for informing who accesses the system.
  if (id == "7A C4 97 80")
    return "Oliver Aagaard Jeppesen";
}
