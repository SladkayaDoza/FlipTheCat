void settings() {
  bool updDisplay = true;
  bool editable = false;
  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();

      oled.setCursor(14, 0);
      oled.print("Frequency: ");
      oled.print(signalDetectionFrequencies[FrequencyPointer]);

      oled.setCursor(14, 1);
      oled.print("Start12bit: ");
      oled.print(start12bitBruteForce);

      oled.setCursor(14, 2);
      oled.print("Beep: ");
      oled.print(pik);

      oled.setCursor(14, 3);
      oled.print("BeepHz: ");
      oled.print(pikHz);

      if (editable) {
        printRightPointer(pointer);
      } else {
        printPointer(pointer);
      }
      oled.update();
    }

    if (ok.click()) {
      if (pointer == 1) {
        start12bitBruteForce = setNumber("12 bit", start12bitBruteForce);
      } else if (pointer == 2) {
        pik = !pik;
        if (pik) setupBuzzer(pikHz);
        objectConfigFile["pik"] = pik;
        saveJsonToFile("/config.json", docConfigFile);
      } else if (pointer == 3) {
        pikHz = setNumber("picHz", pikHz);
        objectConfigFile["pikHz"] = pikHz;
        ledcWriteTone(1, pikHz);
        ledcWrite(1, 0);
        saveJsonToFile("/config.json", docConfigFile);
      } else {
        editable = !editable;
      }
      updDisplay = true;
    }

    if (back.click()) {
      return;
    }

    if (editable) {
      if (up.click()) {
        if (pointer == 0) {
          FrequencyPointer = constrain(FrequencyPointer + 1, 0, SIGNAL_DETECTION_FREQUENCIES_LENGTH - 1);
          updDisplay = true;
        }
      }
      if (down.click()) {
        if (pointer == 0) {
          FrequencyPointer = constrain(FrequencyPointer - 1, 0, SIGNAL_DETECTION_FREQUENCIES_LENGTH - 1);
          updDisplay = true;
        }
      }

    } else {
      if (up.click() or up.hold()) {
        pointer = constrain(pointer - 1, 0, ITEMS - 1);
        updDisplay = true;
      }
      if (down.click() or down.hold()) {
        pointer = constrain(pointer + 1, 0, ITEMS - 1);
        updDisplay = true;
      }
    }
  }
}