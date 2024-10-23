#include <Arduino.h>
#include <ArduinoJson.h>

void RecordSignal(uint8_t pointer, JsonArray sensorArray, JsonObject sens);
int tryRecordSignalToBuffer(JsonArray& array);
void sendSamples(JsonArray array);
