/* Vincent Li s204176
 *  Security System - Webserver
 *  - Connects the LED to a Webserver and a database (firebase)
 *  - Functions as a Security Alarm where you can turn it on / off
 *  - Sends data to our database so others can see the state of the alarm
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library

#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Define Firebase information and wifi setup
#define FIREBASE_HOST "securitysystem-35692-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UuAZ0864SghM66Wt6E8rNL1Okaa7cwYG6M0SJxqq"
#define WIFI_SSID "SSID" //provide ssid (wifi name)
#define WIFI_PASSWORD "Wifi Pw" //wifi password  

int fireStatus;
ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);

const int led = D2;

void handleRoot();
void handleLED();
void handleNotFound();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  // Connect to WiFi network
  Serial.println();
  wifiMulti.addAP("<ssid2>", "<password");  // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("<ssid2>", "<password>");

  Serial.println();
  Serial.print("Connecting ...");
  //WiFi.begin(ssid, password);

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected to ");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("iot")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_POST, handleLED);
  server.onNotFound(handleNotFound);


  // Start the server
  server.begin();
  Serial.println("Server started");

  //Connecting to our firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Connects to google firebase
  if (Firebase.failed())
  {
    Serial.print(Firebase.error());
  }
  else {
    Serial.print("Firebase Connected");
  }
  timeClient.begin(); // After Wifi has been connected
}


void loop() {
  // Check if a client has connected
  server.handleClient();
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Turn Security System on and off</title><meta charset=\"utf-8\" \/> \
      </head><body><h1>Turn Security System on and off</h1> \
      <p> Press the button to turn the security system on and off \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Security system on/off\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
}

void handleLED() {  // If a POST request is made to URI /LED

// Here we make a if statement to the database which is very self-explanatory. If the "ALERT" OR "LISTENING_STATE" is ON, we set it to OFF and vice versa.
  fireStatus = Firebase.getInt("ALERT_STATE");
  if ((Firebase.getInt("ALERT_STATE") == 1) || (Firebase.getInt("LISTENING_STATE") == 1) ) {
    Firebase.setInt("ALERT_STATE", 0);
    Firebase.setInt("LISTENING_STATE", 0);
  } else {
    if ((Firebase.getInt("LISTENING_STATE") == 0) && (Firebase.getInt("ALERT_STATE") == 0)) {
      Firebase.setInt("LISTENING_STATE", 1);
    }
  }

  //Related to the Firebase, so we can see what has last accessed the database, in this case its the webserver.
  //It also tells us at what time we have accessed the database.
  timeClient.update();
  Firebase.setString("lastAccessID", "Webpage");
  Firebase.setString("accessStatus", "Access granted");
  Firebase.setString("accessTime", String(daysOfTheWeek[timeClient.getDay()]) + ", " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));



  digitalWrite(led, !digitalRead(led));     // Change the state of the LED
  server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
