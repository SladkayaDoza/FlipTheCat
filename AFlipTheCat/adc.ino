const float R1 = 50610.0;  // 50 кОм
const float R2 = 76400.0;  // 100 кОм

const float referenceVoltage = 3.348;  // Референсное напряжение для ESP32
const int adcResolution = 4095;  // Разрешение АЦП (12 бит)

float getVolltage() {
  int adcValue = 0;
  for (int i = 0; i < 64; i++) {
    adcValue += analogRead(35);  // 2.339v or 2902
  }
  adcValue /= 64;
  Serial.println(adcValue);
  float measuredVoltage = (adcValue * referenceVoltage) / adcResolution;  // Преобразуем значение АЦП в напряжение
  float inputVoltage = measuredVoltage * ((R1 + R2) / R2);  // Учитываем делитель напряжения

  return inputVoltage;
}
