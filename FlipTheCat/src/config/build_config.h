#pragma once
// =============================================================================
//  FlipTheCat — общие константы и флаги сборки прошивки.
// =============================================================================

#define FTC_FIRMWARE_VERSION "2.0.0"

// Число видимых строк на экране (8 строк по 8px на 128x64 OLED).
#define UI_MENU_ROWS 8

// Максимум ячеек для списков RC/Raw (индексируются строковыми ключами "0".."N").
#define RC_ITEMS_MAX 32

// (число пользовательских выходных GPIO — см. pins.h: GPIO_OUT_PIN_COUNT.
//  Отдельный алиас не заводим: имя GPIO_PIN_COUNT уже занято ESP-IDF.)

// --- Отладка ---
// Раскомментируйте, чтобы включить подробный вывод в Serial (по умолчанию тихо).
// #define FTC_DEBUG_SERIAL

#ifdef FTC_DEBUG_SERIAL
  #define FTC_LOG(x)    Serial.print(x)
  #define FTC_LOGLN(x)  Serial.println(x)
#else
  #define FTC_LOG(x)    do {} while (0)
  #define FTC_LOGLN(x)  do {} while (0)
#endif
