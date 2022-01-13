#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

int index = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte buzzer = 5; //buzzer to arduino pin 5
const byte LED = 2;
int SoundValue = 1000;

const int stepsPerRevolution = 250;  // change this to fit the number of steps per revolution
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup(){

  Serial.begin(115200);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  myStepper.setSpeed(60);

}

void loop(){
 
  alertState();
 
}


void alertState() {

  if (index < 1) {
    for (int i = 0; i <5; i++) {
      myStepper.step(stepsPerRevolution);
      index++; 
    }
  }
  
  tone(buzzer, SoundValue); // Sends 1 KHz sound signal
  digitalWrite(LED,HIGH);
  delay(300);
  noTone(buzzer); // Switches buzzer of
  digitalWrite(LED,LOW);
  delay(300);
  

  lcd.setCursor(1, 0);
  lcd.print("Police is on");
  lcd.setCursor(1, 1);
  lcd.print("their way!");

  
  // Write to Thingsspeak("send a '1' to field 3");
  // Only do it once per function call
  
}
