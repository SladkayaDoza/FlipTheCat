int pinsG[] = { 1, 2, 3 };
byte updOled = 0;

// class realityPins {
//   public:
//     bool mode[pinsCounts];
//     uint8_t mode[pinsCounts];
//     uint8_t frequency[pinsCounts];
//     uint8_t filling[pinsCounts];
// }




char* GPIOPinsMode[7] = {
  "mode: ",
  "frequency: ",
  "Filling: ",
  "",
  "",
  "",
  "",
};


void gpioLay() {
  static uint8_t pointer = 0;
  tk();
  oledUpdater(pointer);

  while (1) {
    tk();
    if (updOled == 1) {
      updOled = 0;
      oledUpdater(pointer);
    }

    if (right.click()) {
      pointer = constrain(pointer + 1, 0, pinsCounts);
      updOled = 1;
    }
    if (left.click()) {
      pointer = constrain(pointer - 1, 0, pinsCounts);
      updOled = 1;
    }
    if (back.click() or back.hold()) break;
    if (ok.click() && pointer != 0) {
      changePin(pointer);
      updOled = 1;
    }
  }
}

void changePin(uint8_t pointer1) {
  static uint8_t pointer = 1;
  tk();
  int updOled = 1;
  while (1) {
    tk();

    if (updOled == 1) {
      updOled = 0;
      oled.clear();
      oled.setCursor(0, 0);
      oled.print("Editing pin: ");
      oled.print(pinsG[pointer1 - 1]);
      for (int i = 1; i < 8; i++) {
        oled.setCursor(0, i);
        oled.print(GPIOPinsMode[i - 1]);
        switch (i - 1) {
          case 0: printOutputPinsModeGPIO(pointer1 - 1); break;
          case 1: nothing(); break;
          case 2: nothing(); break;
          case 3: nothing(); break;
          case 4: nothing(); break;
          case 5: nothing(); break;
          case 6: nothing(); break;
        }
      }
      printRightPointer(pointer);
      oled.update();
    }

    if (back.click() or back.hold()) break;
    if (up.click() or up.hold()) {
      pointer = constrain(pointer - 1, 1, ITEMS);
      updOled = 1;
    }
    if (down.click() or down.hold()) {
      pointer = constrain(pointer + 1, 1, ITEMS);
      updOled = 1;
    }
    if (ok.click() or ok.hold()) {
      switch (pointer1 - 1) {
        case 0:
          switch (pointer - 1) {
            case 0:
              pin1.change();
              if (pin1.getState()) {
                pin1.start();
              } else {
                pin1.setFilling(0);
              }
              OutPinModeG[pointer1 - 1] = !OutPinModeG[pointer1 - 1];
              break;
            case 1:
              pin1.setFrequency(setNumber("frequency", pin1.getFrequency()));
              break;
            case 2:
              pin1.setFilling(setNumber("filling", pin1.getFilling()));
              break;
          }

          break;
        case 1:
          pin2.change();
          OutPinModeG[pointer1 - 1] = !OutPinModeG[pointer1 - 1];
          break;
        case 2:
          pin3.change();
          OutPinModeG[pointer1 - 1] = !OutPinModeG[pointer1 - 1];
          break;
      }

      // pointer = constrain(pointer + 1, 1, ITEMS);
      updOled = 1;
    }
  }
}

void editPinParametr(uint8_t pin, uint8_t cursor) {
  ;
  ;
}

void printOutputPinsModeGPIO(int pointer) {
  if (OutPinModeG[pointer] == 0) {
    oled.print("LOW");
  } else {
    oled.print("HIGH");
  }
}

void oledUpdater(uint8_t pointer) {
  tk();
  oled.clear();
  if (pointer == 0) {
    oled.setCursor(0, 0);
    oled.print("Available pins: ");
    oled.setCursor(14, 1);
    for (int i = 0; i < pinsCounts; i++) {
      oled.print(pinsG[i]);
      if (i != pinsCounts - 1) {
        oled.print(" ");
      }
    }
    oled.print(";");

  } else {
    int pinSelect = pointer - 1;
    oled.setCursor(0, 0);
    oled.print("Selected pin: ");
    oled.print(pinsG[pinSelect]);
    for (int i = 1; i < 8; i++) {
      oled.setCursor(0, i);
      oled.print(GPIOPinsMode[i - 1]);
      switch (i - 1) {
        case 0: printOutputPinsModeGPIO(pointer - 1); break;
        case 1: nothing(); break;
        case 2: nothing(); break;
        case 3: nothing(); break;
        case 4: nothing(); break;
        case 5: nothing(); break;
        case 6: nothing(); break;
      }
    }
  }
  oled.update();
}