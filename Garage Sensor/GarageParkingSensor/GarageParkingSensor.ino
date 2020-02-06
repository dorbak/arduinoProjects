#include <LedControl.h>


#include <LowPower.h>
#include "myMatrix.h"

// LED Matrix Stuff
int DIN = 12;   // DIN pin of MAX7219 module
int CLK = 10;   // CLK pin of MAX7219 module
int CS = 11;    // CS pin of MAX7219 module
int maxInUse = 1;
LedControl m(DIN, CLK, CS, maxInUse); 

// Ultrasonic Sensor Stuff
const int interruptPin = 2; //D2 - Interrupt
const int trigPin = 4;
const int echoPin = 3;
const int powerPin = 5;

// Coloured LED Stuff
const int redpin = 7; // D8 red LED
const int bluepin = 6; // D7 blue LED
const int greenpin = 8 ;// D6 green LED

// Interrupt Timing Stuff
long timeStarted=0;
long timeout = 10000; // 25s Timeout
long currentTime=0;


// Setup
void setup()
{
  pinMode(13, OUTPUT);  // Internal LED
  digitalWrite(13, LOW); // Turn off Internal LED
  m.clearDisplay(0);;
  pinMode(interruptPin, INPUT_PULLUP); //Set up wake up
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(powerPin, OUTPUT);

  pinMode (redpin, OUTPUT);
  pinMode (bluepin, OUTPUT);
  pinMode (greenpin, OUTPUT);
  
  Serial.begin(9600);
}

// Main Loop
void loop()
{
  Serial.println("Ready!");
  attachInterrupt(0,wakeup, LOW);
 
  digitalWrite(13, LOW);
  digitalWrite(powerPin, LOW);

  analogWrite(redpin, 0);
  analogWrite(bluepin, 0);
  analogWrite(greenpin, 0);

  delay(1000);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0);
  awake();
}


void setSprite(byte *sprite){
    for(int r = 0; r < 8; r++){
        Serial.println(sprite[r], BIN);
        m.setRow(0, r, sprite[r]);
    }
}

void awake()
{
  digitalWrite(powerPin, HIGH);
  long currentTime = millis();
  timeStarted = currentTime;
  //Serial.println(timeStarted);
  while (currentTime - timeStarted <= timeout)
  {
   // Serial.print("In loop:");
   // Serial.println(currentTime);
      currentTime = millis();
      digitalWrite(13, HIGH);
      delay (250);
      digitalWrite(13, LOW);
      delay(750);
      readSensor();
  }
  sleep();
}

void sleep()
{
  m.clearDisplay(0);
  m.shutdown(0,true);
  digitalWrite(13,LOW);

}

void readSensor()
{
  
  long duration;
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;

  Serial.print("Distance: ");
  Serial.println(distance);


  if (distance < 12)
  {
    // Red
    analogWrite(redpin, 255);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 0);
    m.clearDisplay(0);
    setSprite(stop);
  
  }
  if (distance >= 12 && distance < 20)
  {
    // Yellow
    analogWrite(redpin, 255);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 255);
    m.clearDisplay(0);
    setSprite(One);

  }
  if (distance >=20)
  {
    // Green
    analogWrite(redpin, 0);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 255);
    
    m.clearDisplay(0);
    setSprite(A);

  }  
}
void wakeup() {
  currentTime = 0;
  timeStarted = 0;
  digitalWrite(powerPin, HIGH);
  m.shutdown(0,false);
  m.setIntensity(0, 8);
  m.clearDisplay(0);
 // m.init(); // MAX7219 initialization
 // m.setIntensity(1); // initial led matrix intensity, 0-15
  
}
