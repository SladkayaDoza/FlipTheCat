#include <ArduinoJson.h>

void saveJsonToFile(const char* filename, JsonDocument& docs);
void readJsonFromFile(const char* filename, JsonDocument& docs);