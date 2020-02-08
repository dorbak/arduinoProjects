#include <LedControl.h>


#include <LowPower.h>
#include "myMatrix.h"

// LED Matrix Stuff
int DIN = 12;   // DIN pin of MAX7219 module
int CLK = 10;   // CLK pin of MAX7219 module
int CS = 11;    // CS pin of MAX7219 module
int maxInUse = 1;
byte packedChars[8];
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
  // Set up Internal LED  
  pinMode(13, OUTPUT);  
  
  // Set up Interrupt for Sensor Wakeup
  pinMode(interruptPin, INPUT_PULLUP); 
  
  // Set up Ultrasonic Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(powerPin, OUTPUT);
  
  // Setup RGB LED
  pinMode (redpin, OUTPUT);
  pinMode (bluepin, OUTPUT);
  pinMode (greenpin, OUTPUT);

  digitalWrite(13, LOW); // Turn off internal LED
  m.clearDisplay(0);  // Clear the LED Maxtrix display

  // Start Serial Connection Monitor
  Serial.begin(9600);
}

// Main Loop
void loop()
{
  Serial.println("Ready!");
  attachInterrupt(0,wakeup, LOW);
  
  digitalWrite(13, LOW);
  
  // Turn off the Ultrasonic Sensor
  digitalWrite(powerPin, LOW);

  // Turn off the RGB LED
  analogWrite(redpin, 0);
  analogWrite(bluepin, 0);
  analogWrite(greenpin, 0);

  // Power off the LED Matrix
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(0);
  awake();
}

// Function to write the sprite to the screen (assumes 8 rows)
void setSprite(byte *sprite){
    for(int r = 0; r < 8; r++){
       // Serial.println(sprite[r], BIN);
        m.setRow(0, r, sprite[r]);
    }
}

void awake()
{
  digitalWrite(powerPin, HIGH);
  long currentTime = millis();
  timeStarted = currentTime;
  int newdistance = 999;
  int distance = 999;
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
      
      newdistance = readSensor();
      if (newdistance != distance)
      {
        timeStarted = currentTime;
        distance = newdistance;
      }
       else
       {
        Serial.println("Distance hasn't changed...");
        Serial.print("Time left: ");
        Serial.println(currentTime - timeStarted);
       }
  }
  sleep();
}

void sleep()
{
  m.clearDisplay(0);
  m.shutdown(0,true);
  digitalWrite(13,LOW);

}

int readSensor()
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

  // Convert distance to sprites...
  // if Greater than 20cm

  //if less than 50cm

  if (distance < 50)
  {
    if (distance > 40 && distance <=50)
      setSprite(circle[0]);
    else if (distance > 30 && distance <=40)
      setSprite(circle[1]);
    else if (distance > 20 && distance <=30)
      setSprite(circle[2]);
    else if (distance >= 12 && distance <=20)
      setSprite(circle[3]);
    else if (distance <12)
      setSprite(circle[4]);
  }
  else
    setSprite(upArrow);
  
  if (distance < 12)
  {
    // Red
    analogWrite(redpin, 255);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 0);
  }
  if (distance >= 12 && distance < 20)
  {
    // Yellow
    analogWrite(redpin, 255);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 255);
  }
  if (distance >=20)
  {
    // Green
    analogWrite(redpin, 0);
    analogWrite(bluepin, 0);
    analogWrite(greenpin, 255);
  }  
  return distance;
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
