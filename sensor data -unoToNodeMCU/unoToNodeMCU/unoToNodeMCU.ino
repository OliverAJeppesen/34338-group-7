/* 
 Sensor Data - Arduino to NodeMCU Serial Communication
 
 Sending LDR and Ultrasonic sensor Data from Arduino to NodeMCU via Serial Communication.
  
 The circuit: 
 * LDR connected to analog pin A5 of arduino.
 * Ultrasonic sensor connected to digital pins 9 & 10 of arduino.
 * NodeMCU connected to digital pins 5 & 6 of arduino.
  
 created 19 January 2022
 by Joshua Sebastian
 
*/

#include <SoftwareSerial.h> //Arduino to NodeMCU library for serial communication
#include <ArduinoJson.h> //JSON library for embedded C/C++

SoftwareSerial nodemcu(5, 6); //Initialise Arduino to NodeMCU (5=Rx & 6=Tx)

//Initialisation of variables
int lightstate=0; //stores the light state based on the threshold set (900) for the LDR analog values
int distancestate=0; //stores the distance state based on the threshold set(50cm) for the distance measured by the ultrasonic sensor
const int trigPin = 10; //trigger pin of ultrasonic sensor connected to digital pin 10 of the Arduino UNO
const int echoPin = 9; //echo pin of ultrasonic sensor connected to digital pin 9 of the Arduino UNO
long duration; 
float distance;

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  pinMode(A5,INPUT); //analog pin A5 set as input for the LDR 
  delay(1000);
  pinMode(trigPin, OUTPUT); // set the trigger pin as an Output
  pinMode(echoPin, INPUT); // set the echo Pin as an Input
  Serial.println("Program started");
}

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject(); //creating JSON object called data
  light(); //Obtain light and distance state status
  ultrasonic();
  Serial.println("-----------------------------------------");
  data["lightstate"] = lightstate; //Assign collected data to JSON Object
  data["distancestate"] = distancestate;
  data.printTo(nodemcu); //Send data to NodeMCU
  jsonBuffer.clear();
  delay(2000);
}


void light() {
  int sensorValue= analogRead(A5); //reading LDR sensor analog values
  if (sensorValue < 900) //setting the LDR analog value as 900 as a threshold to detect if the intruder is using a flashlight to break open the lock
  {
  lightstate=0;
  }
  else
  {
  lightstate=1;
  }
  Serial.print(sensorValue);
  Serial.print("Lightstate: ");
  Serial.println(lightstate);
}

void ultrasonic() {
  digitalWrite(trigPin, LOW); // Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.0343/2; // Convert to cm. 0.0343 is the speed of sound in cm/microsecond
  Serial.print("Distance (cm): "); // Prints the distance on the Serial Monitor
  Serial.println(distance);
  if (distance < 50) //setting distance as 50cm as threshold to detect intruders
  {
  distancestate=1;
  }
  else
  {
  distancestate=0;
  }
  Serial.print("DistanceState: ");
  Serial.println(distancestate);
}
