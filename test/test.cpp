#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_BMP5xx.h>
#include <esp32-hal-timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

const int ledRed = 12;
const int ledYellow = 33;
const int ledGreen= 32;

const int release = 5; 

void flash(int pin);

void setup(){
    Serial.begin(115200);

    //Init LED
    pinMode(ledRed, OUTPUT); 
    pinMode(ledGreen, OUTPUT); 
    pinMode(ledYellow, OUTPUT);
    digitalWrite(ledRed, HIGH); 
    digitalWrite(ledGreen, LOW); 
    digitalWrite(ledYellow, LOW);

    pinMode(release, INPUT_PULLUP);
}

void loop(){
    int yn = digitalRead(release);
    Serial.println(yn);
}
