#include <LowPower.h>

const int interruptPin = 2; //D2 - Interrupt
const int trigPin = 4;
const int echoPin = 3;
const int powerPin = 5;

const int redpin = 8; // D8 red LED
const int bluepin = 6; // D7 blue LED
const int greenpin = 7 ;// D6 green LED

long timeStarted=0;
long timeout = 10000; // 25s Timeout
long currentTime=0;
void setup()
{
  pinMode(13, OUTPUT);  // Internal LED
  digitalWrite(13, LOW); // Turn off Internal LED
  pinMode(interruptPin, INPUT_PULLUP); //Set up wake up
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(powerPin, OUTPUT);

  pinMode (redpin, OUTPUT);
  pinMode (bluepin, OUTPUT);
  pinMode (greenpin, OUTPUT);
  
  Serial.begin(9600);
}

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
    analogWrite(redpin, 255-255);
    analogWrite(bluepin, 255-0);
    analogWrite(greenpin, 255-0);
    
  }
  if (distance >= 12 && distance < 20)
  {
    // Yellow
    analogWrite(redpin, 255-255);
    analogWrite(bluepin, 255-0);
    analogWrite(greenpin, 255-255);
  }
  if (distance >=20)
  {
    // Green
    analogWrite(redpin, 255-0);
    analogWrite(bluepin, 255-0);
    analogWrite(greenpin, 255-255);
 
  }  
}
void wakeup() {
  currentTime = 0;
  timeStarted = 0;
  digitalWrite(powerPin, HIGH);
}
