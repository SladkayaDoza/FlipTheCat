#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

bool RecordSignal(uint8_t pointer, JsonArray sensorArray, JsonObject sens);
int tryRecordSignalToBuffer(JsonArray& array);
void sendSamples(JsonArray array);
