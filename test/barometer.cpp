#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_BMP5xx.h>
#include <esp32-hal-timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

//Servo Init
Servo s1;

// Pick ESP32 GPIOs (change if you want)
const int ledRed = 12;
const int ledYellow = 33;
const int ledGreen= 32;

static const int P1 = 15; //Servo Pin
static const int RELEASE = 5; //Wire 

static const int SERVO_MIN_US = 500;
static const int SERVO_MAX_US = 2400;

//Barometer Init
#define SDA_PIN          21
#define SCL_PIN          22
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP5xx bmp;
bool armed = false;
bool idle = true;
const int arm_height = 55;//140; //ft 
const int drop_height = 40;//60 ; //ft 
float refPressure_hPa; 
float altitude; 

//Prototype Functions 
void flash(int pin); 
void ccw(Servo& a, int target);
void cw(Servo& a, int target);
void neutral(Servo& s, int neutral_pos);

void setup() {
  //Setup Serial for debugging
  //Serial.begin(115200);

 
  pinMode(RELEASE, INPUT_PULLUP);

  //Servo
  s1.setPeriodHertz(50);
  s1.attach(P1, SERVO_MIN_US, SERVO_MAX_US);
  s1.write(45);

  //Debug LEDs 
  pinMode(ledRed, OUTPUT); 
  pinMode(ledGreen, OUTPUT); 
  pinMode(ledYellow, OUTPUT);
  digitalWrite(ledRed, HIGH); 
  digitalWrite(ledGreen, LOW); 
  digitalWrite(ledYellow, LOW);

  //Barometer 
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bmp.begin(BMP5XX_ALTERNATIVE_ADDRESS, &Wire)) {
    Serial.println(F("Sensor not found! Check wiring."));
    flash(ledRed);
    while (1) delay(10);
  }
  //Serial.println(F("BMP5xx found!"));
  bmp.setPowerMode(BMP5XX_POWERMODE_NORMAL);
  bmp.setOutputDataRate(BMP5XX_ODR_240_HZ); //Update rate 
  bmp.setPressureOversampling(BMP5XX_OVERSAMPLING_1X); //Reduced oversampling 
  
  //Warm Up
  for (int i = 0; i < 5; i++) {
    bmp.performReading();
    delay(100);
  }
  bmp.performReading();
  float currentPressure_hPa = bmp.pressure;
  refPressure_hPa = currentPressure_hPa;
  flash(ledGreen);
  //Serial.print(F("Reference pressure set to: "));
  Serial.print(refPressure_hPa, 2);
  //Serial.println(F(" hPa — altitude zeroed"));
  altitude = bmp.readAltitude(refPressure_hPa) * 3.28084;
}

void loop() {
  unsigned long ts = millis();
  bool wire = digitalRead(RELEASE);
  if (bmp.performReading()) {
    altitude = bmp.readAltitude(refPressure_hPa) * 3.28084; 
    if (!armed && altitude > arm_height){
      idle = false;
      armed = true;
      digitalWrite(ledRed, LOW); 
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW); 
    }
    if (armed && altitude < drop_height && wire){
      digitalWrite(ledRed, LOW); 
      digitalWrite(ledGreen, HIGH); 
      digitalWrite(ledYellow, LOW);
      cw(s1, 150);
      //Serial.println("DROPPED"); 
    }
    if (idle){
      digitalWrite(ledRed, HIGH); 
      digitalWrite(ledGreen, LOW); 
      digitalWrite(ledYellow, LOW);
      ccw(s1,90);   
    }
  }
  /*
  Serial.print("Alt: ");
  Serial.print(altitude);
  Serial.print(" | ts: ");
  Serial.print(ts);
  Serial.print(" | armed: ");
  Serial.println(armed);
  */
}

void flash(int pin){
  for (int i =0; i<5; i++){
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
    delay(50); 
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