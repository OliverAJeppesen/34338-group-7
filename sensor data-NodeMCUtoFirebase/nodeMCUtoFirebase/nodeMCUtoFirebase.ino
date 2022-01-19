/* 
 Transmitting sensory data to Google Firebase

 Sending sensor data from the NodeMCU to Google Firebase database with the help of a serial communication setup between the NodeMCU and Arduino UNO.
 The Alert System gets activated when either a flashlight is detected or an intruder is in the proximity of 50 cm from the ultrasonic sensor when the Listening State is set to 1.
 
 The circuit: 
 * Digital pins 5 & 6 of arduino connected to digital pins D5 & D6 of NodeMCU
  
 created 19 January 2022
 by Joshua Sebastian
*/

#include <ESP8266WiFi.h> //Wi-Fi library for ESP8266
#include <FirebaseArduino.h> //Google Firebase Library for Arduino
#include <SoftwareSerial.h> //Arduino to NodeMCU library
#include <ArduinoJson.h> //JSON library for embedded C/C++

// Set login and Wi-Fi credentials to login to Google Firebase database  
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"  
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"  
#define WIFI_SSID "OnePlus 9R"  
#define WIFI_PASSWORD "josh1234"  

SoftwareSerial nodemcu(D6, D5); // pins D6 = Rx & D5 = Tx
//initializing variables
int listeningstate = 0; //reads the listening state from the Firebase database

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //connect to the Wi-Fi
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
      {
    Serial.print(".");
    delay(500);
      }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //connect to the Firebase database using the credentials
  while (!Serial) continue;
}

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu); //parsing the JSON object received from the Arduino UNO
  Serial.println("JSON Object Recieved");
  int lightstate = data["lightstate"]; //collecting light and distance status data and assigning them to variables.
  int distancestate=data["distancestate"];
  Serial.print("lightstate:  ");
  Serial.println(lightstate);
  Serial.print("Distancestate:  ");
  Serial.println(distancestate);
  Serial.println("-----------------------------------------");
  listeningstate = Firebase.getInt("LISTENING_STATE"); //reading the listening status from the Firebase database = 0/1
  if (listeningstate == 1) {
    if(lightstate==1 || distancestate==1){
    Firebase.setInt("ALERT_STATE",1);   //switching ON the Alert state once the person is in the vicinity of the ultrasonic sensor(50cm threshold) or uses the flash light to break open the doors.
  }
 }
}
