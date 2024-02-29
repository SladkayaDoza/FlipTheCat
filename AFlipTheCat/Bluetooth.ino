void selectMouse() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("BLE Mouse");
  oled.update();

  // BleMouse bleMouse;
  // delay(50);
  // btStart();
  // delay(50);

  // bleMouse.begin();
  // oled.setCursor(0, 0);
  // oled.print("BLE Mouse");
  // oled.setCursor(0, 1);
  // oled.print("   Emulation..");
  // oled.update();
  // while (1) {
  //   tk();
  //   if (back.click() or back.hold()) break;
  //   if (bleMouse.isConnected()) {
  //     if (top_left.click()) {
  //       bleMouse.move(0, 0, -1);
  //     }
  //     if (top_right.click()) {
  //       bleMouse.move(0, 0, 1);
  //     }
  //   }
  // }
  // bleMouse.end();
  // btStop();
}


void selectKeyboard() {
  btStart();
  delay(150);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("BLE Keyboard");

  BleKeyboard bleKeyboard("BLE KEY", "FooBar", 100);
  delay(50);
  // btStart();
  delay(50);

  bool butLeft = false;
  bool butRight = false;
  bool init = true;

  bleKeyboard.begin();

  oled.setCursor(0, 1);
  oled.print("   Emulation..");
  oled.update();

  bleKeyboard.releaseAll();

  while (1) {
    tk();
    if (back.click() or back.hold()) break;
    if (bleKeyboard.isConnected()) {
      if (init) {
        init = 0;
        bleKeyboard.releaseAll();
      }
      if (!digitalRead(top_left_pin) && butLeft == false) {
        // Serial.println("left");
        butLeft = true;
        bleKeyboard.press(KEY_LEFT_ARROW);
      }
      if (digitalRead(top_left_pin) && butLeft) {
        butLeft = false;
        bleKeyboard.release(KEY_LEFT_ARROW);
      }

      if (!digitalRead(top_right_pin) && butRight == false) {
        butRight = true;
        bleKeyboard.press(KEY_RIGHT_ARROW);
      }
      if (digitalRead(top_right_pin) && butRight) {
        butRight = false;
        bleKeyboard.release(KEY_RIGHT_ARROW);
      }
    }
  }

  bleKeyboard.end();
  btStop();
}