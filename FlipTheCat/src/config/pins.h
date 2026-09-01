#pragma once
// =============================================================================
//  FlipTheCat — единая карта выводов (pin map)
//  Единственный источник истины для всех GPIO. Не дублируйте #define пинов
//  по другим файлам — подключайте этот заголовок.
// =============================================================================

// --- OLED (I2C, аппаратный Wire; выводы заданы платой ESP32 по умолчанию) ---
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_I2C_CLOCK 800000L

// --- Кнопки-джойстик (резистивная лестница на одном АЦП) ---
#define BUTTONS_ADC_PIN 34

// --- Дополнительные тактовые кнопки ---
#define TOP_LEFT_PIN 0   // альт. вариант разводки — 17
#define TOP_RIGHT_PIN 16

// --- Индикация / звук ---
#define LED_PIN 26
#define BUZZER_PIN 27

// --- Питание ---
#define BATTERY_ADC_PIN 35

// --- CC1101 (VSPI) ---
#define CC1101_SCK_PIN 18
#define CC1101_MISO_PIN 19
#define CC1101_MOSI_PIN 23
#define CC1101_CS_INTERNAL_PIN 5
#define CC1101_CS_EXTERNAL_PIN 13
#define RC_PIN 4  // CC1101 GDO0 / линия данных RCSwitch и raw

// --- Пользовательские выходы GPIO (управляются экраном GPIO) ---
#define GPIO_OUT_PIN_1 32
#define GPIO_OUT_PIN_2 33
#define GPIO_OUT_PIN_3 25
#define GPIO_OUT_PIN_COUNT 3

// -----------------------------------------------------------------------------
//  Совместимость со старыми именами (используются по всему коду).
//  Оставлены как алиасы, чтобы не переписывать каждый вызов сразу.
// -----------------------------------------------------------------------------
#define top_left_pin TOP_LEFT_PIN
#define top_right_pin TOP_RIGHT_PIN
#define led_pin LED_PIN
#define RCPin RC_PIN
