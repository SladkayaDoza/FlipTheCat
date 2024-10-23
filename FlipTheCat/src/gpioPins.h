
#include <Arduino.h>

class OUTPIN {
public:
  OUTPIN(byte pin, int frequency) {
    _pin = pin;
    _frequency = frequency;
    _filling = 128;
    pinMode(_pin, OUTPUT);
  }

  void change() {
    _flag = !_flag;
  }
  void setFrequency(int frequency) {
    _frequency = frequency;
    ledcSetup(0, _frequency, 8);
  }
  void start() {
    ledcSetup(0, _frequency, 8);
    ledcAttachPin(_pin, 0);
    ledcWrite(0, _filling);
  }
  void setFilling(int filling) {
    _filling = filling;
    ledcWrite(0, _filling);
  }
  void off() {
    ledcWrite(0, 0);
  }
  bool getState() {
    return _flag;
  }
  int getFilling() {
    return _filling;
  }
  int getFrequency() {
    return _frequency;
  }
private:
  byte _pin;
  bool _flag;
  int _frequency;
  int _filling;
};

OUTPIN pin1(32, 80000);  // pin and frequency
OUTPIN pin2(33, 80000);
OUTPIN pin3(25, 80000);