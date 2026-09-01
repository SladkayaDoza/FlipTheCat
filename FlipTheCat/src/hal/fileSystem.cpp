#include "SPIFFS.h"
#include <ArduinoJson.h>
#include "build_config.h"

void saveJsonToFile(const char* filename, JsonDocument& docs) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    FTC_LOGLN("Не удалось открыть файл для записи");
    return;
  }
  // Прямой стрим в файл — без промежуточной кучевой String (раньше документ
  // полностью сериализовался в растущую String на каждое сохранение).
  serializeJson(docs, file);
  file.close();
  FTC_LOGLN("JSON сохранён");
}

void readJsonFromFile(const char* filename, JsonDocument& docs) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    FTC_LOGLN("Файл не найден, создаём новый");
    return;
  }

  DeserializationError error = deserializeJson(docs, file);
  if (error) {
    FTC_LOGLN("Ошибка при чтении JSON из файла");
  } else {
    FTC_LOGLN("JSON успешно прочитан из файла");
  }
  file.close();
}
