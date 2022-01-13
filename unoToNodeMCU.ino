
//Sending Data from Arduino to NodeMCU Via Serial Communication
//Arduino code


//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);

//Initialisation of variables
int led = 12;
int lightstate=0;
int soundstate=0;

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  pinMode(led,OUTPUT);
  pinMode(8,INPUT);
  pinMode(A5,INPUT);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  //Obtain sound and light status
  sound();
  light();
  Serial.println("-----------------------------------------");
  
  //Assign collected data to JSON Object
  data["lightstate"] = lightstate;
  data["soundstate"] = soundstate; 

  //Send data to NodeMCU
  data.printTo(nodemcu);
  jsonBuffer.clear();

  delay(2000);
}

void sound() {
  int SensorData=digitalRead(8); 
  if(SensorData==1){
  soundstate=1;
  }
  else{
  soundstate=0;
  }
  Serial.print("Soundstate: ");
  Serial.println(soundstate);
}

void light() {
  int sensorValue= analogRead(A5);
  //delay(1000);
  if (sensorValue < 900)
  {
  digitalWrite(led,LOW);
  lightstate=0;
  }
  else
  {
  digitalWrite(led,HIGH);
  lightstate=1;
  }
  Serial.print("Lightstate: ");
  Serial.println(lightstate);
}
