void saveJsonToFile(const char* filename, JsonDocument& docs) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println("Не удалось открыть файл для записи");
    // return;
  }
  String buffer;
  serializeJson(docs, buffer);
  file.print(buffer);
  file.close();
  Serial.println("JSON успешно сохранен в файл");
  // serializeJson(docs, Serial);
  Serial.println();
  Serial.print(buffer);
}

void readJsonFromFile(const char* filename, JsonDocument& docs) {
  // Serial.println(filename);
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Файл не найден, создаем новый");
    return;
  }

  DeserializationError error = deserializeJson(docs, file);
  if (error) {
    Serial.println("Ошибка при чтении JSON из файла");
  } else {
    Serial.println("JSON успешно прочитан из файла");
    // serializeJson(docs, Serial);
  }

  file.close();
}