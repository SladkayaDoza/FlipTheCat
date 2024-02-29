void saveJsonToFile(char* filename, DynamicJsonDocument& docs) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println("Не удалось открыть файл для записи");
    // return;
  }

  serializeJson(docs, file);
  file.close();
  Serial.println("JSON успешно сохранен в файл");
  // serializeJson(docs, Serial);
  Serial.println();
}

void readJsonFromFile(char* filename, DynamicJsonDocument& docs) {
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