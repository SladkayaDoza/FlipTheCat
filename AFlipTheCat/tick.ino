void tk() {
  int analog = analogRead(34);
  up.tick(analog < 680 && analog > 120);
  down.tick(analog < 2180 && analog > 1620);
  left.tick(analog < 4096 && analog > 3760);
  right.tick(analog < 1140 && analog > 580);
  ok.tick(analog < 2920 && analog > 2360);
  back.tick(analog < 1630 && analog > 1070);
  top_left.tick(!digitalRead(top_left_pin));
  top_right.tick(!digitalRead(top_right_pin));
  Serial.println(analog);  // 280
}