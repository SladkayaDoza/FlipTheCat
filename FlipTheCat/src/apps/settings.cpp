#include <Arduino.h>

#include <display.h>
#include <tick.h>
#include <fileSystem.h>
#include <global_vars.h>
#include <pointer.h>
#include <button.h>
#include <buzzer.h>
#include <tools.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>




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
      oled.print(objectConfigFile["externalCCModule"].as<bool>() ? "External" : "Internal");

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
        objectConfigFile["externalCCModule"] = !objectConfigFile["externalCCModule"].as<bool>();
        ELECHOUSE_cc1101.setSpiPin(18, 19, 23, (objectConfigFile["externalCCModule"].as<bool>() ? 13 : 5));
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
      // На экране настроек ровно 5 строк (0..4). Раньше курсор ограничивался
      // общим ITEMS(8) и заезжал на пустые строки 5..7 (OK там переключал editable).
      if (up.click() or up.step()) {
        pointer = constrain(pointer - 1, 0, 4);
        updDisplay = true;
      }
      if (down.click() or down.step()) {
        pointer = constrain(pointer + 1, 0, 4);
        updDisplay = true;
      }
    }
  }
}

// Экран настройки полосы приёма (RxBW) с водопадом активности эфира.
//
// Раньше отрисовка водопада выполнялась в ОТДЕЛЬНОЙ FreeRTOS-задаче на ядре 1,
// а главный цикл на другом ядре менял те же tiles[]/updDisplay/RxBW_Raw без
// синхронизации. Это давало сразу три проблемы: гонку за общими данными,
// утечку семафора (создавался на каждый вход, не удалялся) и удаление задачи
// (vTaskDelete) в момент, когда она держала мьютекс/шину I2C.
//
// Водопад перерисовывается лишь раз в секунду, поэтому второе ядро не нужно:
// рисуем прямо в главном цикле. Так все три проблемы исчезают, а подсчёт
// фронтов на ~несколько мс раз в секунду (время oled.update) для грубой
// визуализации активности несущественен.
void settingsRawBW() {
  setupRx();
  float tempRxBW = RxBW_Raw;
  bool updDisplay = true;
  uint32_t last_time = millis();
  int count = 0;
  uint16_t tiles[16] = {0};
  bool last_state = 0;

  while (1) {
    tk();

    if (digitalRead(RCPin) != last_state && millis() - last_time > DEAD_TIME_IIC) {
      last_state = !last_state;
      count++;
    }

    if (millis() - last_time > 1000) {
      addElementToFront(tiles, 16, count);
      count = 0;
      updDisplay = true;
      last_time = millis();
    }

    if (up.click() or up.step()) {
      RxBW_Raw += 10;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = true;
    }
    if (right.click() or right.step()) {
      RxBW_Raw += 1;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = true;
    }
    if (down.click() or down.step()) {
      RxBW_Raw -= 10;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = true;
    }
    if (left.click() or left.step()) {
      RxBW_Raw -= 1;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      updDisplay = true;
    }

    if (back.click() or back.hold()) {
      RxBW_Raw = tempRxBW;
      ELECHOUSE_cc1101.setRxBW(RxBW_Raw);
      ELECHOUSE_cc1101.goSleep();
      return;
    }
    if (ok.click() or ok.hold()) {
      ELECHOUSE_cc1101.goSleep();
      return;
    }

    if (updDisplay) {
      updDisplay = false;
      oled.clear();
      oled.home();
      oled.setCursor(14, 0);
      oled.print("RxBW: ");
      oled.print(RxBW_Raw);
      for (int i = 0; i < 16; i++) {
        oled.rect(i * 8 + 1, 64, i * 8 + 7, 62 - tiles[15 - i]);
      }
      oled.update();
    }
  }
}