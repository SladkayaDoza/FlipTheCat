#include <Arduino.h>
#include <ArduinoJson.h>
#include <global_vars.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <display.h>
#include <button.h>
#include <tick.h>
#include <tools.h>
#include <buzzer.h>

// RECORDING SIGNAL PARAMETERS
#define MAX_EMPTY_RECORDING_CYCLES 32  // 32 RESET CYCLES
#define MINIMUM_RECORDED_TRANSITIONS 64
#define MINIMUM_RECORDTIME_MICROSECONDS 16000
#define MINIMUM_LENGHT_RECORDED_SIGNAL_MICROSECONDS 80
#define MAX_LENGHT_RECORDED_SIGNAL 4096
#define MAX_TRANSITION_TIME_MICROSECONDS 32000
long lastRecordDuration = 0;

int tryRecordSignalToBuffer(JsonArray& array);


// Возвращает true, если запись завершена; false — если пользователь прервал (back).
bool RecordSignal(uint8_t pointer, JsonArray sensorArray, JsonObject sens) {
  String point = String(pointer);

  setupRx();
  ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
  oled.clear();
  oled.home();
  oled.print("Recording..");
  oled.update();

  int transitions = 0;
  // Сбрасываем ГЛОБАЛЬНЫЙ lastRecordDuration (его обновляет
  // tryRecordSignalToBuffer). Раньше здесь была локальная переменная, которая
  // затеняла глобальную, поэтому условие по времени (< 16000 мкс) было мёртвым
  // и запись коротких сигналов (<64 переходов) никогда не завершалась.
  lastRecordDuration = 0;
  while (transitions < MINIMUM_RECORDED_TRANSITIONS &&
         lastRecordDuration < MINIMUM_RECORDTIME_MICROSECONDS) {
    transitions = tryRecordSignalToBuffer(sensorArray);
    tk();
    if (back.click() or back.hold()) return false;  // прервано пользователем
  }

  if (pik) beep();

  oled.clear();
  oled.home();
  oled.print(transitions);
  oled.println(" Transitions Recorded");
  oled.update();
  unsigned long tmmm = millis();  // было uint16_t — усечение millis() ломало паузу
  while (millis() - tmmm < 1000) {
    tk();
  }

  String name = setName("SetName");
  sens[point]["size"] = transitions;
  sens[point]["name"] = name;
  sens[point]["frequency"] = FrequencyPointer;
  return true;
}
// передать json лист

int tryRecordSignalToBuffer(JsonArray& array) {
  // setupRx();
  // RESET
  byte currentInput = 0;
  int sign = -1;
  int64_t attempts = 0;
  int maxAttempts = 32;
  int recordedTransitions = 0;
  int64_t recordingStarted = esp_timer_get_time();

  for (recordedTransitions = 0; recordedTransitions < MAX_LENGHT_RECORDED_SIGNAL; recordedTransitions++) {
    // RECORD TRANSITION TIMES
    int64_t transitionTime = 0;
    int64_t readingStarted = esp_timer_get_time();

    while (transitionTime < MAX_TRANSITION_TIME_MICROSECONDS) {
      transitionTime = esp_timer_get_time() - readingStarted;
      if (digitalRead(RCPin) != currentInput) {
        //BREAK THE LOOP IF THE PIN STATE CHANGES
        break;
      }
    }

    digitalWrite(led_pin, digitalRead(RCPin));

    int transitionValue;
    if (transitionTime >= MAX_TRANSITION_TIME_MICROSECONDS) {
      transitionValue = MAX_TRANSITION_TIME_MICROSECONDS * sign;
      array.add(transitionValue);
      // RESET ITERATOR WHEN -32000 WAS RECORDED FOR THE FIRST TIME
      if (recordedTransitions == 0) {
        recordedTransitions = -1;
        array.clear();
        attempts++;
        if (attempts > maxAttempts) {
          //Serial.println("No signal detected!");
          return -1;
        }
      } else {
        // -32000 WAS RECORDED AFTER SOME POSITIVES VALUES --> END OF SIGNAL
        attempts++;
        if (attempts > MAX_EMPTY_RECORDING_CYCLES) {
          //Serial.println("End of signal detected!");
          break;
        }
      }

    } else if (transitionTime >= MINIMUM_LENGHT_RECORDED_SIGNAL_MICROSECONDS) {
      transitionValue = transitionTime * sign;
      array.add(transitionValue);
      currentInput = !currentInput;
      if (currentInput) {
        sign = 1;
      } else {
        sign = -1;
      }
    }
  }

  int64_t recordingEnded = esp_timer_get_time();
  lastRecordDuration = (long)(recordingEnded - recordingStarted);

  return recordedTransitions;
}

void sendSamples(JsonArray array) {

  setupTx();

  FTC_LOGLN("Transmitting " + String(array.size()) + " Samples");

  int pulseUs = 0;  // было `int delay` — затеняло Arduino delay()
  byte n = 0;

  for (int i = 0; i < array.size(); i++) {
    // TRANSMIT
    n = 1;
    pulseUs = array[i];
    if (pulseUs < 0) {
      // DONT TRANSMIT
      pulseUs = pulseUs * -1;
      n = 0;
    }

    digitalWrite(RCPin, n);
    delayMicroseconds(pulseUs);
  }

  // STOP TRANSMITTING
  digitalWrite(RCPin, 0);
  FTC_LOGLN("Transmission completed.");
}
