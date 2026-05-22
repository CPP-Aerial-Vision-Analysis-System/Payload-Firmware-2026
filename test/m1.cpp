#include "esp32-hal-timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "Arduino.h"

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t isrCounter = 0;

void ARDUINO_ISR_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void setup() {
  Serial.begin(115200);

  timerSemaphore = xSemaphoreCreateBinary();

  // timer 0, prescaler 80 (1 tick = 1us at 80MHz), count up
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // alarm every 1,000,000 ticks = 1 second, autoreload
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  Serial.println(F("Timer started — firing every 1 second"));
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    portENTER_CRITICAL(&timerMux);
    uint32_t count = isrCounter;
    portEXIT_CRITICAL(&timerMux);

    Serial.print(F("Tick #"));
    Serial.println(count);
  }
}