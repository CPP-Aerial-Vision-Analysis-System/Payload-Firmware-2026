#include <Arduino.h>
#include <ESP32Servo.h>

Servo s1, s2, s3, s4;

// Pick safe ESP32 GPIOs (change if you want)
static const int P1 = 13;
static const int P2 = 12;
static const int P3 = 14;
static const int P4 = 27;
static const int INT = 5; 
static const int BTN = 35; 

// Typical servo pulse range (adjust if needed)
static const int SERVO_MIN_US = 500;
static const int SERVO_MAX_US = 2400;

void ccw(Servo& a, Servo& b, int target);
void cw(Servo& a, Servo& b, int target);
void neutral(Servo& s, int neutral_pos);

void setup() {
  Serial.begin(115200);

  pinMode(BTN, INPUT_PULLUP);
  //pinMode(INT, INPUT_PULLUP);
  
  // Standard hobby servos use 50 Hz
  s1.setPeriodHertz(50);
  s2.setPeriodHertz(50);
  s3.setPeriodHertz(50);
  s4.setPeriodHertz(50);

  s1.attach(P1, SERVO_MIN_US, SERVO_MAX_US);
  s2.attach(P2, SERVO_MIN_US, SERVO_MAX_US);
  s3.attach(P3, SERVO_MIN_US, SERVO_MAX_US);
  s4.attach(P4, SERVO_MIN_US, SERVO_MAX_US);

  // Optional: start at neutral
  s1.write(90);
  s2.write(90);
  s3.write(90);
  s4.write(90);
}

void loop() {
  int pressed = (digitalRead(BTN) == LOW);  
  //int pressed = (digitalRead(BTN) == HIGH);
  Serial.println(pressed);
  if(pressed == 1){
    //ccw(s3, s4, 20);
    cw(s3, s4, 170);

    cw(s1, s2, 170);
    //ccw(s1, s2, 20);
    //ccw(s1, s2, 90);
    delay(1000);
  }else{
    //neutral(s1,90);
    //neutral(s2,90);
    //neutral(s3,90);
    //neutral(s4,90);
  }
}

// Moves clockwise to absolute target (target > current)
void cw(Servo& a, Servo& b, int target) {
  int pos = a.read();
  for (int i = pos; i <= target; i++) {
    a.write(i);
    b.write(i);
    delay(1);
  }
}

// Moves counterclockwise to absolute target (target < current)
void ccw(Servo& a, Servo& b, int target) {
  int pos = a.read();
  for (int i = pos; i >= target; i--) {
    a.write(i);
    b.write(i);
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