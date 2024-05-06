void setupBuzzer(int freq) {
  ledcSetup(1, freq, 8);
  ledcAttachPin(27, 1);
  ledcWrite(1, 0);
}
void beep() {
  if (pik) {
    ledcWrite(1, 128);
    pikState = 1;
    TimePik = esp_timer_get_time();
  }
}