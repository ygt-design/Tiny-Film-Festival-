#include <HCSR04.h>
#include "Arduino_LED_Matrix.h"
#include "newMail.h"
#include "loading.h"
#include "areYouARobot.h"
#include "unsub.h"
#include "uSure.h"
#include "networkError.h"

byte triggerPin = 14;
byte echoPin = 15;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

unsigned long lastRead = 0;
int rate = 30;

const int numReadings = 10; 
float readings[numReadings];
int readIndex = 0; 
float total = 0; 
float averageDistance = 0; 

float threshold1 = 3;
float threshold2 = 5;
float threshold3 = 7;
float threshold4 = 9;
float threshold5 = 12;



int currentState = 0;
int previousState = -1; 

ArduinoLEDMatrix ledScreen;  
boolean loopAnimation = true;

void setup () {
  Serial.begin(9600);
  ledScreen.begin();
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0.0;
  }
}

void loop () {
  readDistanceSensor();
  determineState();

  if (currentState != previousState) 
  {
    switch (currentState) {
      case 0:
        ledScreen.loadSequence(networkError);
        break;
      case 1:
        ledScreen.loadSequence(loading);
        break;
      case 2:
        ledScreen.loadSequence(newMail);
        break;
      case 3:
        ledScreen.loadSequence(unsub);
        break;
      case 4:
        ledScreen.loadSequence(uSure);
        break;
      case 5:
        ledScreen.loadSequence(areYouARobot);
        break;
    }
    ledScreen.play(loopAnimation);
    
  }
  previousState = currentState;
}

void readDistanceSensor() {
  unsigned long currentTime = millis();
  
  if ((currentTime - lastRead) >= rate) {
    lastRead = currentTime;
    float distanceMeasured = distanceSensor.measureDistanceCm();
    
    if (distanceMeasured > 0) { 
      total -= readings[readIndex]; 
      readings[readIndex] = distanceMeasured; 
      total += readings[readIndex]; 
      readIndex = (readIndex + 1) % numReadings; 
      
      averageDistance = total / numReadings; 
      
      Serial.print("Raw Distance: ");
    Serial.print(distanceMeasured);
    Serial.print(" cm, Rolling Average: ");
    Serial.print(averageDistance);
    Serial.println(" cm");
    }
  }
}

void determineState() {
  // Determine the state based on the average distance
  if (averageDistance <= threshold1) {
    currentState = 1;
  } else if (averageDistance > threshold1 && averageDistance <= threshold2) {
    currentState = 2;
  }else if (averageDistance > threshold2 && averageDistance <= threshold3) {
    currentState = 3;
  }else if (averageDistance > threshold3 && averageDistance <= threshold4) {
    currentState = 4;
  }else if (averageDistance > threshold4 && averageDistance <= threshold5) {
    currentState = 5;
  } else {
    currentState = 0;
  }
}
