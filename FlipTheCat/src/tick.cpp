#include <button.h>
#include <global_vars.h>

void tk() {
  int analog = analogRead(34);
  up.tick(analog < 1372 && analog > 0);
  right.tick(analog < 1625 && analog > 1372);
  back.tick(analog < 1967 && analog > 1625);
  down.tick(analog < 2477 && analog > 1967);
  ok.tick(analog < 3442 && analog > 2477);
  left.tick(analog < 4096 && analog > 3442);
  top_left.tick(!digitalRead(top_left_pin));
  top_right.tick(!digitalRead(top_right_pin));
  if (pik && pikState && esp_timer_get_time() - TimePik > 10000) {
    pikState = 0;
    ledcWrite(1, 0);
  }
  // Serial.println(analog);
}