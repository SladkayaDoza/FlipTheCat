#pragma once

#include <Arduino.h>
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include "pins.h"          // единая карта выводов (все #define пинов)
#include "build_config.h"  // версии, флаги, константы

#include <RCSwitch.h>
extern RCSwitch mySwitch;

extern void setupRx();
extern void setupTx();

// --- Радио-состояние (последний принятый RC-код) ---
extern unsigned long store1;
extern uint8_t bitR1;
extern uint8_t protoc;

// --- Бипер ---
extern bool pik;
extern uint64_t TimePik;
extern bool pikState;
extern int pikHz;

// --- Raw ---
extern float RxBW_Raw;

// --- Настройки ---
extern uint8_t FrequencyPointer;
extern int start12bitBruteForce;

// --- Пользовательские выходные GPIO ---
// Единый размер: раньше существовали два запутанно похожих счётчика
// pinsCount(4)/pinsCounts(3). Оставлен один — pinsCounts == GPIO_PIN_COUNT.
constexpr uint8_t pinsCounts = GPIO_OUT_PIN_COUNT;  // = 3
extern int OutPinModeG[pinsCounts];

// --- Анализатор частот ---
#define SIGNAL_DETECTION_FREQUENCIES_LENGTH 20
extern float signalDetectionFrequencies[SIGNAL_DETECTION_FREQUENCIES_LENGTH];
extern int detectedRssi;
extern float detectedFrequency;
extern int fineRssi;
extern float fineFrequency;
extern int minRssi;
extern bool RECORDING_SIGNAL;
extern bool RxTxMode;

// --- Граница навигации по спискам RC/Raw (макс. число ячеек) ---
extern int RC_ITEMS;

// --- Хранилище JSON (документы + корневые объекты) ---
extern JsonDocument docConfigFile;
extern JsonDocument doc;
extern JsonDocument docRC;
extern JsonObject objectConfigFile;
extern JsonObject objectDoc;
extern JsonObject objectRC;
