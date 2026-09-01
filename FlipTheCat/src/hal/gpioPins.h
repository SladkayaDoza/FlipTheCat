#pragma once
#include <Arduino.h>
#include "pins.h"

// Каждый выход получает СВОЙ LEDC-канал (см. gpioPins.cpp), поэтому пины
// независимы и не конфликтуют с бипером. Бипер — канал 1 (таймер 0); выходы —
// каналы 2/3/4. Раньше все выходы делили канал 0, а он с бипером (ch1) сидел на
// одном таймере 0, из-за чего частоты влияли друг на друга.
class OUTPIN {
public:
  OUTPIN(byte pin, int frequency, uint8_t channel)
      : _pin(pin), _flag(false), _frequency(frequency), _filling(128), _channel(channel) {
    pinMode(_pin, OUTPUT);
  }

  void change() { _flag = !_flag; }
  void setFrequency(int frequency) {
    _frequency = frequency;
    ledcSetup(_channel, _frequency, 8);
  }
  void start() {
    ledcSetup(_channel, _frequency, 8);
    ledcAttachPin(_pin, _channel);
    ledcWrite(_channel, _filling);
  }
  void setFilling(int filling) {
    _filling = filling;
    ledcWrite(_channel, _filling);
  }
  void off() { ledcWrite(_channel, 0); }
  bool getState() { return _flag; }
  int getFilling() { return _filling; }
  int getFrequency() { return _frequency; }

private:
  byte _pin;
  bool _flag;
  int _frequency;
  int _filling;
  uint8_t _channel;
};

// Определения объектов — в gpioPins.cpp (иначе multiple definition при повторном
// подключении заголовка во второй .cpp — классический ODR).
extern OUTPIN pin1;
extern OUTPIN pin2;
extern OUTPIN pin3;
