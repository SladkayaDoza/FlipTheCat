#include "esp_adc_cal.h"

// Определите пин, к которому подключен аналоговый сигнал
#define ADC_PIN ADC1_CHANNEL_7

// Константы для калибровки
#define DEFAULT_VREF    1100  // Используйте 1100 в качестве опорного напряжения по умолчанию
#define NO_OF_SAMPLES   64    // Количество выборок для усреднения

// Структура для хранения калибровочных значений
esp_adc_cal_characteristics_t *adc_chars;

// Переменные для хранения среднего значения ADC
uint32_t adc_reading = 0;

// Функция для инициализации ADC и калибровки
void setup_adc() {
  // Настройка ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_PIN, ADC_ATTEN_DB_0);

  // Аллоцирование памяти для структуры
  adc_chars = (esp_adc_cal_characteristics_t *) calloc(1, sizeof(esp_adc_cal_characteristics_t));
  
  // Выполнение калибровки
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

float getVolltage() {
  adc_reading = 0;
  // Выполнение нескольких выборок и усреднение результатов
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
    adc_reading += adc1_get_raw(ADC_PIN);
  }
  adc_reading /= NO_OF_SAMPLES;

  // Преобразование сырых данных в напряжение в мВ
  uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);

  return voltage*2.09;
}
