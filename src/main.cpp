#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_LSM6DSO32.h>

//Servo Init
Servo s1;
//Servo Pin
static const int P1 = 15; 
//Servo Hz
static const int SERVO_MIN_US = 500;
static const int SERVO_MAX_US = 2400;

//Snag Wire 
static const int RELEASE = 5; //ZERO == NOT RELEASED

//Accel Init
Adafruit_LSM6DSO32 dso32;

// LED GPIO
const int ledRed = 12;
const int ledYellow = 33;
const int ledGreen= 32;

//Vars
bool armed;

//Prototype Functions 
void flash(int pin, int del);
void ccw(Servo& a, int target);
void cw(Servo& a, int target);
void neutral(Servo& s, int neutral_pos);

void setup(){
    //Begin Serial for debugging 
    Serial.begin(115200);
    while (!Serial)
        delay(10);  

    //Servo
    s1.setPeriodHertz(50);
    s1.attach(P1, SERVO_MIN_US, SERVO_MAX_US);
    s1.write(45);

    //Snag wire 
    pinMode(RELEASE, INPUT_PULLUP);

    //Begin accel over i2c
    if (!dso32.begin_I2C()) {
        while (1) {
        delay(10);
        }
    }

    //Debug LEDs 
    pinMode(ledRed, OUTPUT); 
    pinMode(ledGreen, OUTPUT); 
    pinMode(ledYellow, OUTPUT);
    digitalWrite(ledRed, HIGH); 
    digitalWrite(ledGreen, LOW); 
    digitalWrite(ledYellow, LOW);
}

void loop(){
    Serial.println(digitalRead(RELEASE)); 

}

//Flashes LED with del seconds inbetween 
void flash(int pin, int del){
  for (int i =0; i<5; i++){
    digitalWrite(pin, HIGH);
    delay(del);
    digitalWrite(pin, LOW);
    delay(del); 
  }
}

// Moves clockwise to absolute target (target > current)
void cw(Servo& a, int target) {
  int pos = a.read();
  for (int i = pos; i <= target; i++) {
    a.write(i);
    delay(1);
  }
}

// Moves counterclockwise to absolute target (target < current)
void ccw(Servo& a, int target) {
  int pos = a.read();
  for (int i = pos; i >= target; i--) {
    a.write(i);
    delay(1);
  }
}

// Holds / returns to neutral position
void neutral(Servo& s, int neutral_pos) {
  int pos = s.read();
  if (pos < neutral_pos) {
    for (int i = pos; i <= neutral_pos; i++) {
      s.write(i);
      delay(10);
    }
  } else if (pos > neutral_pos) {
    for (int i = pos; i >= neutral_pos; i--) {
      s.write(i);
      delay(10);
    }
  }
}