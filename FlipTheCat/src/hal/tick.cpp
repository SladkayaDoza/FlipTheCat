#include <button.h>
#include <global_vars.h>

void tk() {
  int analog = analogRead(BUTTONS_ADC_PIN);
  // Диапазоны стыкуются без зазоров: нижняя граница включительна (>=),
  // иначе значения ровно на границе (1800/2060/2360/2850/3623) не попадали ни в одну кнопку.
  up.tick(analog < 1800 && analog > 0);         // 1650
  right.tick(analog < 2060 && analog >= 1800);  // 1950
  back.tick(analog < 2360 && analog >= 2060);   // 2170
  down.tick(analog < 2850 && analog >= 2360);   // 2550
  ok.tick(analog < 3623 && analog >= 2850);     // 3150
  left.tick(analog <= 4096 && analog >= 3623);  // 4096
  top_left.tick(!digitalRead(top_left_pin));
  top_right.tick(!digitalRead(top_right_pin));
  if (pik && pikState && esp_timer_get_time() - TimePik > 10000) {
    pikState = 0;
    ledcWrite(1, 0);
  }
  // Serial.println(analog);
}