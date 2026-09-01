#include <Arduino.h>
#include "pins.h"
#include "build_config.h"

const float R1 = 50610.0;              // 50 кОм (верхнее плечо делителя)
const float R2 = 76400.0;              // нижнее плечо делителя
const float referenceVoltage = 3.348;  // опорное напряжение АЦП ESP32
const int adcResolution = 4095;        // 12-битное АЦП

// Напряжение батареи меняется медленно, поэтому кешируем результат и
// пересчитываем не чаще раза в секунду. Раньше главный экран делал 64
// блокирующих analogRead + Serial.println на КАЖДУЮ перерисовку.
static float cachedVoltage = 0.0f;
static uint32_t lastSampleMs = 0;
static bool haveSample = false;

float getVolltage() {
  uint32_t now = millis();
  if (haveSample && (now - lastSampleMs) < 1000) return cachedVoltage;

  float adcValue = 0;
  for (int i = 0; i < 16; i++) {
    adcValue += analogRead(BATTERY_ADC_PIN);
  }
  adcValue /= 16;

  float measuredVoltage = (adcValue * referenceVoltage) / adcResolution;
  float inputVoltage = measuredVoltage * ((R1 + R2) / R2);  // учёт делителя

  cachedVoltage = inputVoltage;
  lastSampleMs = now;
  haveSample = true;
  return inputVoltage;
}
