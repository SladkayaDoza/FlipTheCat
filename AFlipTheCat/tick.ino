void tk() {
  int analog = analogRead(34);
  up.tick(analog < 700 && analog > 120);
  down.tick(analog < 2500 && analog > 1825);
  left.tick(analog < 4096 && analog > 3480);
  right.tick(analog < 1245 && analog > 700);
  ok.tick(analog < 3480 && analog > 2500);
  back.tick(analog < 1825 && analog > 1245);
  top_left.tick(!digitalRead(top_left_pin));
  top_right.tick(!digitalRead(top_right_pin));
  // Serial.println(analog);
}