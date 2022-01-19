/*
  Alert System

  This program constantly checks a database on google Firebase,
  and activates the alertState function if the alert state is equal to one.
  The alert state function blinks the LED and sends out a buzzing signal for 200 miliseconds,
  and then waits for 200 miliseconds. This keeps going repeatedly until the alert state is swithced off.
  When the function is called the LCD display also show that the police is on their way,
  as well as locks the door by turning the step motor.

  Main responsible for this code:
  Niklas Spanggaard Gundersen - s204191

*/

// Include the right libraries
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


// Define Firebase information and wifi setup
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"
#define WIFI_SSID "Nikklas iphone" //provide ssid (wifi name)
#define WIFI_PASSWORD "niklas01" //wifi password


// Hardware constants
int fireStatus;
const int LED = D4;
const byte buzzer = D8; //buzzer to arduino pin 5
int SoundValue = 1000;
int x1 = 0;
int alert = 1;
int count = 1;

// Constants used to delcare time and date
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Initializes LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initializes step motor
const int stepsPerRevolution = 250;  // number of steps per revolution
Stepper myStepper(stepsPerRevolution, D3, D5, D6, D7);//

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT); // Set LED - pin D4 as an output
  pinMode(buzzer, OUTPUT); // Set buzzer - pin D8 as an output
  myStepper.setSpeed(60); // Sets the speed of the motor

  // Initializes the LCD and clears it
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP()); // prints the local IP address

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Connects to google firebase
  if (Firebase.failed()) {
    Serial.print(Firebase.error());
  }
  else {
    Serial.print("Firebase Connected");
  }
  timeClient.begin(); // Starts the client that operates the time and date
}

void loop() {
  fireStatus = Firebase.getInt("ALERT_STATE"); // Checks the Firebase database repeatedly, and gets the value of "ALERT_STATE"
  if (fireStatus == 1) {
    Serial.println("Alert state is on");
    alertState(); // If ALERT_STATE is 1 call the alertState function
  }
  else if (fireStatus == 0) {
    Serial.println("Alert state is off");
    x1 = 0; // Sets x1 to 0, which is used in the alertState function
    return;
  }
  else {
    Serial.println("Command Error! Please send 0/1");
  }
}

void alertState() {
  if (x1 < 1) { // Run this if statement only once
    timeClient.update();  // Sends the time the alert system was triggered to Firebase
    Firebase.setString("AlertTime", String(daysOfTheWeek[timeClient.getDay()]) + ", " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
    Firebase.setInt("numberOfAlertState", count); // Update the number of times alertState has been activated
    count++;
    for (int i = 0; i < 5; i++) { // Turns the stepmotor one round
      myStepper.step(stepsPerRevolution);
      delay(5);
      x1++;
    }
  }

  tone(buzzer, SoundValue); // Sends 1 KHz sound signal
  digitalWrite(LED, HIGH);
  delay(200);
  noTone(buzzer); // Switches buzzer of
  digitalWrite(LED, LOW);
  delay(200);

  lcd.setCursor(1, 0);
  lcd.print("Police is on"); // Prints on the LCD
  lcd.setCursor(1, 1);
  lcd.print("their way!");
}
