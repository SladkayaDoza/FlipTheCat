#pragma once
#include <Arduino.h>

void oledUpdater(uint8_t pointer);
void gpioLay();

void changePin(uint8_t pointer1);

void printOutputPinsModeGPIO(int pointer);
void printOutputPinsFillingGPIO();
void printOutputPinsFrequencyGPIO();

