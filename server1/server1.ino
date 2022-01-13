#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ThingSpeak.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);



const char* ssid = "Nikklas iphone";
const char* pass = "niklas01";
WiFiClient client;

unsigned long channelID = 1629295; //your TS channal
const char * APIKey = "51ERDDI9QTYN7DSF"; //your TS API
const char* server2 = "api.thingspeak.com";

const byte buzzer = D8; //buzzer to arduino pin 5

int SoundValue = 1000;
int x1 = 0;
int x2 = 0;
int alert = 1;

const int stepsPerRevolution = 250;  // change this to fit the number of steps per revolution
Stepper myStepper(stepsPerRevolution, D3, D5, D6, D7);


WiFiServer server(80);
IPAddress IP(192, 168, 4, 15);
IPAddress mask = (255, 255, 255, 0);

const int LED = D4;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Wemos_AP", "Wemos_comm");
  WiFi.softAPConfig(IP, IP, mask);
  server.begin();

  pinMode(LED, OUTPUT);


  pinMode(buzzer, OUTPUT); // Set buzzer - pin D0 as an output

  lcd.init();
  lcd.backlight();
  lcd.clear();


  myStepper.setSpeed(60);

  Serial.println();
  Serial.println("accesspoint_bare_01.ino");
  Serial.println("Server started.");
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  Serial.print("MAC:"); Serial.println(WiFi.softAPmacAddress());
}
void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  digitalWrite(LED, LOW);
  String request = client.readStringUntil('\r');
  Serial.println("********************************");
  Serial.println("From the station: " + request);
  client.flush();
  Serial.print("Byte sent to the station: ");
  Serial.println(client.println(request + "farvel" + "\r"));

  if (request == "hello whats up!") {
    for (int i = 0; i < 10; i++) {
      alertState();

      ThingSpeak.begin(client);
      client.connect(server2, 80); //connect(URL, Port)
      ThingSpeak.setField(6, alert); //set data on the X graph
      ThingSpeak.writeFields(channelID, APIKey);//post everything to TS
      Serial.print(alert);
      client.stop();
    }
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
