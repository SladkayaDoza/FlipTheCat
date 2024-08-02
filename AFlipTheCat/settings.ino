#define DEAD_TIME_IIC 160

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
      
      oled.setCursor(14, 4);
      oled.print("CC module: ");
      oled.print(objectConfigFile["externalССModule"].as<bool>() ? "External" : "Internal");

      if (editable) {
        printRightPointer(pointer);
      } else {
        printPointer(pointer);
      }
      oled.update();
    }

    if (ok.click()) {
      updDisplay = true;
      switch (pointer) {
      case 1:
        start12bitBruteForce = setNumber("12 bit", start12bitBruteForce);
        break;
      case 2:
        pik = !pik;
        if (pik) setupBuzzer(pikHz);
        objectConfigFile["pik"] = pik;
        saveJsonToFile("/config.json", docConfigFile);
        break;
      case 3:
        pikHz = setNumber("picHz", pikHz);
        objectConfigFile["pikHz"] = pikHz;
        ledcWriteTone(1, pikHz);
        ledcWrite(1, 0);
        saveJsonToFile("/config.json", docConfigFile);
        break;
      case 4:
        objectConfigFile["externalССModule"] = !objectConfigFile["externalССModule"].as<bool>();
        ELECHOUSE_cc1101.setSpiPin(18, 19, 23, (objectConfigFile["externalССModule"].as<bool>() ? 13 : 5));
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setMHZ(signalDetectionFrequencies[FrequencyPointer]);
        mySwitch.enableReceive(RCPin);
        ELECHOUSE_cc1101.SetRx();
        ELECHOUSE_cc1101.goSleep();
        saveJsonToFile("/config.json", docConfigFile);
        break;
      default:
        editable = !editable;
      }
    }

    if (back.click()) {
      return;
    }

    if (editable) {
      if (up.click() or up.step()) {
        switch (pointer) {
        case 0:
          FrequencyPointer = constrain(FrequencyPointer + 1, 0, SIGNAL_DETECTION_FREQUENCIES_LENGTH - 1);
          updDisplay = true;
          break;
        }
      }

      if (down.click() or down.step()) {
        switch (pointer) {
        case 0:
          FrequencyPointer = constrain(FrequencyPointer - 1, 0, SIGNAL_DETECTION_FREQUENCIES_LENGTH - 1);
          updDisplay = true;
          break;
        }
      }

    } else {
      if (up.click() or up.step()) {
        pointer = constrain(pointer - 1, 0, ITEMS - 1);
        updDisplay = true;
      }
      if (down.click() or down.step()) {
        pointer = constrain(pointer + 1, 0, ITEMS - 1);
        updDisplay = true;
      }
    }
  }
}

SemaphoreHandle_t displayMutex;

struct TaskParameters {
  bool *updDisplay;
  uint16_t *tiles;
};

void whileReadChanges(void *pvParameters) {
  TaskParameters *params = (TaskParameters *)pvParameters;
  bool *updDisplay = params->updDisplay;
  uint16_t *tiles = params->tiles;

  while (1) {
    if (xSemaphoreTake(displayMutex, portMAX_DELAY)) {
      if (*updDisplay) {
        *updDisplay = false;
        
        oled.clear();
        oled.home();

        oled.setCursor(14, 0);
        oled.print("RxBW: ");
        oled.print(RxBW_Raw);

        for (int i = 0; i < 16; i++) {
          oled.rect(i*8+1, 64, i*8+7, 62-tiles[15-i]);
        }
        
        oled.update();
      }
      xSemaphoreGive(displayMutex);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);  // Небольшая задержка для стабилизации работы
  }
}

void settingsRawBW() {
  setupRx();
  float tempRxBW = RxBW_Raw;
  digitalRead(RCPin);
  bool updDisplay = true;
  uint32_t last_time = millis();
  int count = 0;
  uint16_t tiles[16] = {0};
  bool last_state = 0;

  TaskHandle_t reader = NULL;
  TaskParameters params = {&updDisplay, tiles};
  displayMutex = xSemaphoreCreateMutex();

  // Создание задачи на Core 1
  xTaskCreatePinnedToCore(
    whileReadChanges,      // Функция задачи
    "Reader",    // Название задачи
    10000,            // Размер стека для задачи
    (void *)&params,             // Параметры задачи
    1,                // Приоритет задачи
    &reader,             // Указатель на созданную задачу (не используется)
    1                 // Номер ядра (0 для Core 0, 1 для Core 1)
  );

  while (1) {
    tk();

    if (digitalRead(RCPin) != last_state && millis() - last_time > DEAD_TIME_IIC) {
      last_state = !last_state;
      count++;
    }

    if (millis() - last_time > 1000) {
      addElementToFront(tiles, 16, count);
      count = 0;
      updDisplay = 1;
      last_time = millis();
    }

    if (up.click() or up.step()) {
      RxBW_Raw += 10;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = 1;
    }
    if (right.click() or right.step()) {
      RxBW_Raw += 1;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = 1;
    }
    if (down.click() or down.step()) {
      RxBW_Raw -= 10;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = 1;
    }
    if (left.click() or left.step()) {
      RxBW_Raw -= 1;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = 1;
    }

    if (back.click() or back.hold()) {
      RxBW_Raw = tempRxBW;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      if (reader != NULL) {
        vTaskDelete(reader);  // Удаляем задачу
        reader = NULL;
      }
      ELECHOUSE_cc1101.goSleep();
      return;
    }

    if (ok.click() or ok.hold()) {
      if (reader != NULL) {
        vTaskDelete(reader);  // Удаляем задачу
        reader = NULL;
      }
      ELECHOUSE_cc1101.goSleep();
      return;
    }
  }
}