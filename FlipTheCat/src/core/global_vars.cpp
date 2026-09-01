#include <global_vars.h>

// Радио-состояние
unsigned long store1 = 0;
uint8_t bitR1 = 0;
uint8_t protoc = 0;

// Бипер
bool pik = 0;
uint64_t TimePik = esp_timer_get_time();
bool pikState = 0;
int pikHz = 2700;

// Raw
float RxBW_Raw = 256;

// Настройки
uint8_t FrequencyPointer = 10;
int start12bitBruteForce = 1;

// Пользовательские выходные GPIO
int OutPinModeG[pinsCounts] = {0};

// Анализатор частот
float signalDetectionFrequencies[SIGNAL_DETECTION_FREQUENCIES_LENGTH] = {
    300.00, 303.87, 304.25,  310.00, 315.00, 318.00, 390.00,
    418.00, 433.07, 433.42,  433.92, 434.42, 434.77, 438.90,
    868.3,  868.35, 868.865, 868.95, 915.00, 925.00};
int detectedRssi = -100;
float detectedFrequency = 0.0;
int fineRssi = -100;
float fineFrequency = 0.0;
int minRssi = -75;
bool RECORDING_SIGNAL = false;
bool RxTxMode = true;

// Граница навигации по спискам RC/Raw
int RC_ITEMS = 32;
