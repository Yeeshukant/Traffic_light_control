/*
  Traffic light control
  
  This program controls the traffic light of a square intersection.
  It controls two LEDs for each road - Red and green, with Yellow wait
  light demonstated as both intermediate state where both red and green
  LEDs are off due to less number of digital pins on arduino.

  This program doesn't use the simple delay() function for keeping the
  LEDs on for required time, but rather uses interrupt based millis()
  function to keep a track of time passed and present time without keeping
  the processor busy. This was done to add the additional feature for 
  allowing an emergency (say an ambulance waiting) 
  at road 3
                
                || # road 2
    #road 1 ==       ==  # road 3
                || # road 4
      
  As on Arduino Uno rev3 only pin 2 and 3 support external hardware
  interrupt, so this can be scaled for emergency at all four roads if
  required. Further, since our program doesn't use a delay() function
  it can simultaneously perform any other task if required.

  This program was developed using Platform.io and is therefore written
  in C++, but can easily be modified accordingly for compilation using
  Arduino IDE. A simple google search will give you the answer for how.

  created 7 Nov 2022

  by Yeeshukant Singh

  code availabe at
  
  https://github.com/Yeeshukant/Traffic_light_control

*/
#include <Arduino.h>

// Defining the pins for different signals outputs - where LEDs are connected
int signal1[] = {5, 6};
int signal2[] = {7, 8};
int signal3[] = {9, 10};
int signal4[] = {11, 12};

unsigned long currTime = 0;
unsigned long prevTime =0;           // tracks time stamp of last loop function ending time
const long redDelay= 5000;           // Time for which green light should remain on - 5 secs
const long yellowDelay= 2000;        // Time for which yellow light should remain on (intermediate) - 2 secs
const int buttonPin=2;               // External emergency hardware interrupt pin defined
const int ledPin=13;                 // Built_In LED - used only for checking whethter button got pressed and connection was ok
volatile bool flag = false;          // flag which is set when interrupt occures

// Declaration of state functions
void sig1();
void sig2();
void sig3();
void sig4();

// Declaration of interrupt function
void button_ISR();

void setup() {
  Serial.begin(9600); // only for debugging purpose - no special use in this program
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < 2; i++) {
    pinMode(signal1[i], OUTPUT);
    pinMode(signal2[i], OUTPUT);
    pinMode(signal3[i], OUTPUT);
    pinMode(signal4[i], OUTPUT);
  }

  // This is how an interrupt is defined in arduino
  // attachInterrupt(pin, function to execute, when to change);
  attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, LOW);
}


void loop() {

  currTime = millis(); // keeps track of current time
  
  // waiting times used to give conditions on when to execute some task
  // by comparing with time elapsed
  unsigned long currwaitTime = redDelay, prevwaitTime=0;

  // signal 1
  if (currTime-prevTime<currwaitTime){
    sig1();
  }

  /*intermediate or yellow light state
    as soon as the prev green signal time is over, check if the flag is set
    and if it is set then there is an emergency at road 3 and give it the
    priority to pass. The time is set for 3 secs and here we used a delay function
    as our function has only one road where we can acknowledge an emergency
    but we can also do it using the millis function to keep a track of any other
    interrupt.
  */
  else if((currTime-prevTime>=currwaitTime) &&(currTime-prevTime<currwaitTime+yellowDelay)){
    if(flag){
      sig1();
      delay(3000);
      flag=false;
    }
    digitalWrite(signal1[1], LOW); // start from left
    Serial.println("#1 wait");
  }
  
  // updating previous and current waiting times
  prevwaitTime=currwaitTime+yellowDelay;
  currwaitTime=prevwaitTime+redDelay;
  if((currTime-prevTime>prevwaitTime)&&(currTime-prevTime<=currwaitTime)){
    sig2();
  }
  else if((currTime-prevTime>=currwaitTime) &&(currTime-prevTime<currwaitTime+yellowDelay)){
    if(flag){
      digitalWrite(signal2[1], LOW); // first switch off the current on green led
      sig1();                        // then call LEDs for emergency lane
      delay(3000);                   // delay 3 sec
      digitalWrite(signal1[1], LOW); // turn off the interrupt triggered green LED
      flag=false;
    }
    digitalWrite(signal2[1], LOW);    // start from left
    Serial.println("#2 wait");
  }
  
  // updating previous and current waiting times
  prevwaitTime=currwaitTime+yellowDelay;
  currwaitTime=prevwaitTime+redDelay;
  if((currTime-prevTime>prevwaitTime)&&(currTime-prevTime<=currwaitTime)){
    sig3();
  }
  else if((currTime-prevTime>=currwaitTime) &&(currTime-prevTime<currwaitTime+yellowDelay)){
    if(flag){
      digitalWrite(signal3[1], LOW);
      sig1();
      delay(3000);
      digitalWrite(signal1[1], LOW);
      flag=false;
    }
    digitalWrite(signal3[1], LOW);
    Serial.println("#3 wait");
  }
  
  // updating previous and current waiting times
  prevwaitTime=currwaitTime+yellowDelay;
  currwaitTime=prevwaitTime+redDelay;
  if((currTime-prevTime>prevwaitTime)&&(currTime-prevTime<=currwaitTime)){
    sig4();
  }
  
  else if((currTime-prevTime>=currwaitTime) &&(currTime-prevTime<currwaitTime+yellowDelay)){
    if(flag){
      digitalWrite(signal4[1], LOW);
      sig1();
      delay(3000);
      digitalWrite(signal1[1], LOW);
      flag=false;
    }
    digitalWrite(signal4[1], LOW);
    Serial.println("#4 wait");
  }
  
  // updating previous and current waiting times
  prevwaitTime=currwaitTime+yellowDelay;
  currwaitTime=prevwaitTime+redDelay;
  // update previous time only after one full cycle rotation.
  if(currTime-prevTime>prevwaitTime){
     prevTime=currTime;
    Serial.println("#reset");
  }
}

// Interrupt service routine....this should be as short and precise
// as possible. It cannot take any attributes
void button_ISR(){
  
  Serial.println("////////////Interrupt////////////////////");
  digitalWrite(ledPin, HIGH);
  flag=true;

}

void sig1(){
  digitalWrite(signal1[1], HIGH);
  digitalWrite(signal1[0], LOW);
  digitalWrite(signal2[0], HIGH);
  digitalWrite(signal3[0], HIGH);
  digitalWrite(signal4[0], HIGH);
  Serial.println("#1");
}
void sig2(){
  digitalWrite(signal1[0], HIGH);
  digitalWrite(signal2[1], HIGH);
  digitalWrite(signal2[0], LOW);
  digitalWrite(signal3[0], HIGH);
  digitalWrite(signal4[0], HIGH);
  Serial.println("#2");
}
void sig3(){
  digitalWrite(signal1[0], HIGH);
  digitalWrite(signal2[0], HIGH);
  digitalWrite(signal3[1], HIGH);
  digitalWrite(signal3[0], LOW);
  digitalWrite(signal4[0], HIGH);
  Serial.println("#3");
}
void sig4(){
  digitalWrite(signal1[0], HIGH);
  digitalWrite(signal2[0], HIGH);
  digitalWrite(signal3[0], HIGH);
  digitalWrite(signal4[1], HIGH);
  digitalWrite(signal4[0], LOW);
  Serial.println("#4");
}