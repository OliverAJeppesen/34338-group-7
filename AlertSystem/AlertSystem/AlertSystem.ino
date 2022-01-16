#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

// Firebase definitions
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"
#define WIFI_SSID "Nikklas iphone" //provide ssid (wifi name)
#define WIFI_PASSWORD "niklas01" //wifi password

// Constants;
int fireStatus;
const int LED = D4;
const byte buzzer = D8; //buzzer to arduino pin 5
int SoundValue = 1000;
int x1 = 0;
int alert = 1;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Step motor
const int stepsPerRevolution = 250;  // change this to fit the number of steps per revolution
Stepper myStepper(stepsPerRevolution, D3, D5, D6, D7);

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin D0 as an output
  myStepper.setSpeed(60);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
  if (Firebase.failed())
  {
    Serial.print(Firebase.error());
  }
  else {
    Serial.print("Firebase Connected");
  }
}

void loop() {
  fireStatus = Firebase.getInt("ALERT_STATE");
  if (fireStatus == 1) {
    Serial.println("Alert state is on");
    alertState();
  }
  else if (fireStatus == 0) {
    Serial.println("Alert state is off");
    x1 = 0;
    return;
  }
  else {
    Serial.println("Command Error! Please send 0/1");
  }
}

void alertState() {

  if (x1 < 1) {
    for (int i = 0; i < 5; i++) {
      myStepper.step(stepsPerRevolution);
      delay(5);
      x1++;
    }
  }

  tone(buzzer, SoundValue); // Sends 1 KHz sound signal
  digitalWrite(LED, HIGH);
  delay(300);
  noTone(buzzer); // Switches buzzer of
  digitalWrite(LED, LOW);
  delay(300);

  lcd.setCursor(1, 0);
  lcd.print("Police is on");
  lcd.setCursor(1, 1);
  lcd.print("their way!");

}
