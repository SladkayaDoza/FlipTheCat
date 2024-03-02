// RECORDING SINGAL PARAMERS
#define MAX_EMPTY_RECORDING_CYCLES 32  // 32 RESET CYCLES
#define MINIMUM_RECORDED_TRANSITIONS 64
#define MINIMUM_RECORDTIME_MICROSECONDS 16000
#define MAX_LENGHT_RECORDED_SIGNAL 4096
#define MAX_TRANSITION_TIME_MICROSECONDS 32000
int recordedSignal[MAX_LENGHT_RECORDED_SIGNAL];
int recordedSamples = 0;
long lastRecordDuration = 0;


void RecordSignal(uint8_t pointer, JsonObject sens) {
  String point = String(pointer);

  setupRx();
  oled.clear();
  oled.home();
  oled.print("Recording..");
  oled.update();

  JsonArray sensorArray = sens[point].createNestedArray("RawData");

  int i, transitions = 0;
  long lastRecordDuration = 0;
  while (transitions < MINIMUM_RECORDED_TRANSITIONS && lastRecordDuration < MINIMUM_RECORDTIME_MICROSECONDS) { /*&& (getOperationMode() == OPERATIONMODE_PERISCOPE  || getOperationMode() == OPERATIONMODE_RECORD_SIGNAL))*/
    transitions = tryRecordSignalToBuffer(sensorArray);
    tk();
    if(back.click() or back.hold()) return;
  }

  oled.clear();
  oled.home();
  oled.print(transitions);
  oled.println(" Transitions Recorded");
  oled.update();
  delay(1000);

  String name = setName("SetName");
  sens[point]["name"] = name;
  sens[point]["frequency"] = FrequencyPointer;
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
      // RESET ITERATOR WHEN -3200 WAS RECORDED FOR THE FIRST TIME
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

    } else {
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

  Serial.println("Transmitting " + String(array.size()) + " Samples");

  int delay = 0;
  unsigned long time;
  byte n = 0;

  // Serial.println(String(sensorArray));
  serializeJson(array, Serial);

  for (int i = 0; i < array.size(); i++) {
    // TRANSMIT
    n = 1;
    delay = array[i];
    if (delay < 0) {
      // DONT TRANSMIT
      delay = delay * -1;
      n = 0;
    }

    digitalWrite(RCPin, n);
    delayMicroseconds(delay);
  }

  // STOP TRANSMITTING
  digitalWrite(RCPin, 0);
  Serial.println("Transmission completed.");
}