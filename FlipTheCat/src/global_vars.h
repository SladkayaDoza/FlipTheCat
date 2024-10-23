#include <Arduino.h>
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>

#include <RCSwitch.h>
extern RCSwitch mySwitch;

extern void setupRx();
extern void setupTx();

#define top_right_pin 16
#define top_left_pin 17
#define led_pin 26
#define RCPin 4

extern unsigned long store1;
extern uint8_t bitR1;
extern uint8_t protoc;
extern bool pik;
extern uint64_t TimePik;
extern bool pikState;
extern int pikHz;
extern float RxBW_Raw;

// SETTINGS
extern uint8_t FrequencyPointer;
extern int start12bitBruteForce;

extern uint8_t pinsCount;         // numPins + 1 -- this value is pins + main Available pins
const uint8_t pinsCounts = 3;
extern int OutPinModeG[pinsCounts];

// SINGAL DETECTION
#define SIGNAL_DETECTION_FREQUENCIES_LENGTH 20
extern float signalDetectionFrequencies[SIGNAL_DETECTION_FREQUENCIES_LENGTH];
extern int detectedRssi;
extern float detectedFrequency;
extern int fineRssi;
extern float fineFrequency;
extern int minRssi;
extern bool RECORDING_SIGNAL;
extern bool RxTxMode;


extern int ITEMS;
extern int RC_ITEMS;

extern JsonDocument docConfigFile;
extern JsonDocument doc;
extern JsonDocument docRC;
extern JsonObject objectConfigFile;
extern JsonObject objectDoc;
extern JsonObject objectRC;