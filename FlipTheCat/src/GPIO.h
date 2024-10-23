#include <Arduino.h>

void oledUpdater(uint8_t pointer);
void gpioLay();

void changePin(uint8_t pointer1);

void editPinParametr(uint8_t pin, uint8_t cursor);

void printOutputPinsModeGPIO(int pointer);
void printOutputPinsFillingGPIO();
void printOutputPinsFrequencyGPIO();

