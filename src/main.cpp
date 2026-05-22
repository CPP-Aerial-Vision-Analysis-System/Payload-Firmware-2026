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
static const int RELEASE = 5; 

//Accel Init
Adafruit_LSM6DSO32 dso32;

// LED GPIO
const int ledRed = 12;
const int ledYellow = 33;
const int ledGreen= 32;

//Vars
bool armed;

//Prototype Functions 
void flash(int pin);
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

}

void flash(int pin){

}

void ccw(Servo& a, int target){

}

void cw(Servo& a, int target){

}

void neutral(Servo& a, int target){

}