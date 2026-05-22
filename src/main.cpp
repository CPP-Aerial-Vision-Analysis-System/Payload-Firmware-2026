#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_LSM6DSO32.h>
#include <math.h>

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

//Timer
unsigned long startTime = 0;
bool timerRunning = false;

//Vars
bool armed;
bool release; 
float accel_z; 
const float ACCEL_THRESHOLD = 1.0; 

//Prototype Functions 
void flash(int pin, int del);
void ccw(Servo& a, int target);
void cw(Servo& a, int target);
void neutral(Servo& s, int neutral_pos);
void startTimer();

void setup(){
    //Begin Serial for debugging 
    Serial.begin(115200);
    while (!Serial)
        delay(10);  

    //Servo
    s1.setPeriodHertz(50);
    s1.attach(P1, SERVO_MIN_US, SERVO_MAX_US);
    ccw(s1,90);  

    //Snag wire 
    pinMode(RELEASE, INPUT_PULLUP);

    //Accel
    //Begin over i2c
    if (!dso32.begin_I2C()) {
        while (1) {
        delay(10);
        }
    }
    //Set ranges 
    dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
    dso32.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

    //Debug LEDs 
    pinMode(ledRed, OUTPUT); 
    pinMode(ledGreen, OUTPUT); 
    pinMode(ledYellow, OUTPUT);
    digitalWrite(ledRed, HIGH);  
    digitalWrite(ledGreen, LOW); 
    digitalWrite(ledYellow, LOW);
    
    //Armed? 
    armed = digitalRead(RELEASE); 
    release = false; 
}

void loop(){
    //Collect Accel events 
    sensors_event_t gyro;
    sensors_event_t temp;
    sensors_event_t accel;
    //Get data Accel
    dso32.getEvent(&accel, &gyro, &temp);
    accel_z = accel.acceleration.z; 
    
    //Get data armed 
    armed = digitalRead(RELEASE); 

    //Detect free fall
    if (armed && abs(accel_z) < ACCEL_THRESHOLD && !release){
        Serial.println("Timer started");
        release = true; 
        startTimer();
    }
    if (timerRunning && millis() - startTime >= 5000 && release) {
        timerRunning = false;
        digitalWrite(ledRed, LOW); 
        digitalWrite(ledGreen, HIGH); 
        digitalWrite(ledYellow, LOW);
        Serial.println("Parachute Released");
        cw(s1, 150);
    }
    
    /*
    if (abs(accel_z) < ACCEL_THRESHOLD){
        Serial.println("Free Fall Detected");
    }
    */ 

    //Serial.print("Accel: ");
    //Serial.println(accel_z);
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

void startTimer() {
    startTime = millis();
    timerRunning = true;
}
